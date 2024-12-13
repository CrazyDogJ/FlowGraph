// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestGlobalComponent.h"

#include "FlowExtraFunctionLibrary.h"
#include "FlowNode_QuestFinish.h"
#include "FlowNode_QuestInfo.h"
#include "FlowSubsystem.h"
#include "Net/UnrealNetwork.h"

UQuestGlobalComponent::UQuestGlobalComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuestGlobalComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UQuestGlobalComponent, QuestFlowStateList)
	DOREPLIFETIME(UQuestGlobalComponent, GoalInfoList)
}

void UQuestGlobalComponent::MarkGoalDirty(UFlowNode_QuestCommon* QuestCommonNodePtr, TEnumAsByte<EGoalState> GoalState)
{
	auto NodeTemplate = Cast<UFlowNode_QuestCommon>(
		QuestCommonNodePtr->GetFlowAsset()->GetTemplateAsset()->GetNode(QuestCommonNodePtr->GetGuid()));
	auto Ptr = GoalInfoList.GoalInfos.IndexOfByPredicate([&](const FGoalInfo& GoalInfo)
	{
		return GoalInfo.QuestCommonNodeDefault == NodeTemplate;
	});
	
	if (Ptr >= 0)
	{
		GoalInfoList.GoalInfos[Ptr].GoalDesc = QuestCommonNodePtr->GetGoalDesc();
		GoalInfoList.GoalInfos[Ptr].GoalState = GoalState;
		GoalInfoList.MarkItemDirty(GoalInfoList.GoalInfos[Ptr]);
	}
	else
	{
		auto NewGoalInfo = FGoalInfo(NodeTemplate, QuestCommonNodePtr->GetGoalDesc(), GoalState);
		GoalInfoList.GoalInfos.Add(NewGoalInfo);
		GoalInfoList.MarkItemDirty(NewGoalInfo);
	}
	OnRep_GoalInfoList();
}

void UQuestGlobalComponent::AcceptQuest(UFlowAsset_Quest* QuestFlow)
{
	if (!QuestFlow)
	{
		return;
	}
	
	// Accept on server
	if (GetOwnerRole() == ROLE_Authority)
	{
		auto NewInstance = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>()->StartRootFlow(this, QuestFlow, false);
		if (NewInstance)
		{
			auto NewItem = FQuestFlowState(QuestFlow, QFS_Ongoing);
			QuestFlowStateList.QuestStates.Add(NewItem);
			QuestFlowStateList.MarkItemDirty(NewItem);
			OnRep_QuestFlowStateList();
		}
	}
}

TArray<UFlowAsset*> UQuestGlobalComponent::GetOngoingQuestInstances()
{
	if (auto Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{
		auto Instances = Subsystem->GetRootInstancesByOwner(this);
		return Instances.Array();
	}
	return TArray<UFlowAsset*>();
}

EQuestFlowState UQuestGlobalComponent::GetQuestFlowState(const UFlowAsset* FlowTemplate)
{
	if (!FlowTemplate)
	{
		return QFS_Invalid;
	}
	
	auto Item = QuestFlowStateList.QuestStates.FindByPredicate([&](const FQuestFlowState& Other)
	{
		return Other.QuestFlowTemplate == FlowTemplate;
	});

	if (Item)
	{
		return Item->QuestFlowState;
	}
	
	return QFS_Invalid;
}

void UQuestGlobalComponent::SetSelectedQuestFlow(UFlowAsset* FlowAsset)
{
	SelectedQuestFlow = FlowAsset;
	SelectedQuestFlowChanged.Broadcast();
}

void UQuestGlobalComponent::GetSelectedQuestFlowInfo(TArray<FGoalInfo>& QuestInfos, FText& QuestName)
{
	TArray<FGoalInfo> Result;
	
	if (!SelectedQuestFlow)
	{
		QuestInfos = Result;
		return;
	}

	QuestName = Cast<UFlowAsset_Quest>(SelectedQuestFlow)->QuestName;
	
	for (auto Itr : GoalInfoList.GoalInfos)
	{
		if (Itr.QuestCommonNodeDefault->GetFlowAsset() == SelectedQuestFlow)
		{
			Result.Add(Itr);
		}
	}
	QuestInfos = Result;
}

void UQuestGlobalComponent::GetSelectedFinishedQuestFlow(TArray<FFinishedGoalState>& QuestGoals)
{
	TArray<FFinishedGoalState> Result;
	
	if (!SelectedQuestFlow)
	{
		QuestGoals = Result;
		return;
	}

	for (auto Itr : QuestFlowStateList.QuestStates)
	{
		if (Itr.QuestFlowTemplate == SelectedQuestFlow)
		{
			QuestGoals = Itr.Nodes;
			return;
		}
	}
}

void UQuestGlobalComponent::NotifyGoalNodes(TSubclassOf<UFlowNode_QuestCommon> QuestGoalClass, UObject* Object1, UObject* Object2)
{
	for (const auto Flow : GetOngoingQuestInstances())
	{
		const auto ActiveNodes = Flow->GetActiveNodes();
		for (auto ActiveGoalNode : ActiveNodes)
		{
			if (ActiveGoalNode->GetClass()->IsChildOf(QuestGoalClass))
			{
				Cast<UFlowNode_QuestCommon>(ActiveGoalNode)->OnNotify(Object1, Object2);
			}
		}
	}
}

FQuestSaveData UQuestGlobalComponent::GetQuestSaveData()
{
	FQuestSaveData Result;
	for (auto Itr : QuestFlowStateList.QuestStates)
	{
		if (Itr.QuestFlowState == QFS_Finished || Itr.QuestFlowState == QFS_Failed)
		{
			Result.FinishedQuestFlowAssetSaveData.Add(Itr.QuestFlowTemplate, FFinishedQuestState(Itr.QuestFlowState, Itr.FinishNodeGuid, Itr.Nodes));
		}
	}
	for (auto Itr : GetOngoingQuestInstances())
	{
		if (auto QuestItr = Cast<UFlowAsset_Quest>(Itr))
		{
			Result.OngoingQuestFlowAssetSaveData.Add(QuestItr->GetTemplateAsset(), QuestItr->SaveQuestInstance());
		}
	}
	Result.bValid = Result.OngoingQuestFlowAssetSaveData.Num() > 0;
	return Result;
}

bool UQuestGlobalComponent::LoadQuestSaveData(FQuestSaveData SaveData)
{
	if (!SaveData.bValid)
	{
		return false;
	}
	
	for (auto Itr : SaveData.OngoingQuestFlowAssetSaveData)
	{
		for (auto Itr1 : GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>()->GetRootInstances().Array())
		{
			if (Itr1.Value->GetTemplateAsset() == Itr.Key)
			{
				Itr1.Value->FinishFlow(EFlowFinishPolicy::Abort);
			}
			break;
		}
		
		if (auto NewInstance = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>()->CreateRootFlow(this, Itr.Key, false))
		{
			if (auto NewQuestInstance = Cast<UFlowAsset_Quest>(NewInstance))
			{
				NewQuestInstance->LoadQuestInstance(Itr.Value);
			}
		}
	}

	QuestFlowStateList.QuestStates.Empty();
	for (auto Pair : SaveData.OngoingQuestFlowAssetSaveData)
	{
		auto NewState = FQuestFlowState(Pair.Key,
			QFS_Ongoing,
			FGuid());
		QuestFlowStateList.QuestStates.Add(NewState);
		QuestFlowStateList.MarkItemDirty(NewState);
	}
	for (auto Pair : SaveData.FinishedQuestFlowAssetSaveData)
	{
		auto NewState = FQuestFlowState(Pair.Key,
			Pair.Value.State,
			Pair.Value.FinishNodeGuids,
			Pair.Value.Nodes);
		QuestFlowStateList.QuestStates.Add(NewState);
		QuestFlowStateList.MarkItemDirty(NewState);
	}
	OnRep_QuestFlowStateList();
	OnRep_GoalInfoList();
	return true;
}

//void UQuestGlobalComponent::UpdateGoalsDesc(UFlowAsset* QuestFlowInstance, TArray<FGoalInfo>& ModifyGoals)
//{
//	if (QuestFlowInstance)
//	{
//		for (auto ActiveGoalNode : QuestFlowInstance->GetActiveNodes())
//		{
//			if (auto QuestCommon = Cast<UFlowNode_QuestCommon>(ActiveGoalNode))
//			{
//				auto DefaultNode = Cast<UFlowNode_QuestCommon>(QuestFlowInstance->GetTemplateAsset()->GetNode(QuestCommon->GetGuid()));
//				ModifyGoals.Add(FGoalInfo(DefaultNode, QuestCommon->GetGoalDesc(), QuestCommon->bGoalFinished));
//			}
//		}
//	}
//}

void UQuestGlobalComponent::OnRep_QuestFlowStateList()
{
	if (QuestFlowStateList.QuestStates.Last().QuestFlowState == QFS_Ongoing)
	{
		SetSelectedQuestFlow(QuestFlowStateList.QuestStates.Last().QuestFlowTemplate);
	}
	RecordedQuestFlowsChanged.Broadcast();
}

void UQuestGlobalComponent::OnRep_GoalInfoList()
{
	SelectedQuestFlowChanged.Broadcast();
}
