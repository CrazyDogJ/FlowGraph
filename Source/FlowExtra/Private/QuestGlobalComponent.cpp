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

bool UQuestGlobalComponent::IsQuestFlowValid(UFlowAsset* QuestFlowTemplate)
{
	// Check is a quest flow asset
	auto ConnectedInfoNodes = QuestFlowTemplate->GetNodesInExecutionOrder(QuestFlowTemplate->GetDefaultEntryNode(), UFlowNode_QuestInfo::StaticClass());
	auto ConnectedFinishNodes = QuestFlowTemplate->GetNodesInExecutionOrder(QuestFlowTemplate->GetDefaultEntryNode(), UFlowNode_QuestFinish::StaticClass());
	
	if (ConnectedInfoNodes.Num() != 1)
	{
		UE_LOG(LogTemp, Error, TEXT("You are trying to start a invalid quest flow. Please check whether %s has ONLY ONE Quest Info node."), *QuestFlowTemplate->GetName());
	}

	if (ConnectedInfoNodes.Num() != 1)
	{
		UE_LOG(LogTemp, Error, TEXT("You are trying to start a invalid quest flow. Please check whether %s has ONLY ONE Quest Finish node."), *QuestFlowTemplate->GetName());
	}
	
	return ConnectedInfoNodes.Num() == 1 && ConnectedFinishNodes.Num() == 1;
}

void UQuestGlobalComponent::MarkGoalDirty(UFlowNode_QuestCommon* QuestCommonNodePtr, bool bNeedRemoved)
{
	auto NodeTemplate = Cast<UFlowNode_QuestCommon>(QuestCommonNodePtr->GetFlowAsset()->GetTemplateAsset()->GetNode(QuestCommonNodePtr->GetGuid()));
	auto Ptr = GoalInfoList.GoalInfos.IndexOfByPredicate([&](const FGoalInfo& GoalInfo)
	{
		return GoalInfo.QuestCommonNodeDefault == NodeTemplate;
	});
	
	if (Ptr >= 0)
	{
		if (bNeedRemoved)
		{
			GoalInfoList.GoalInfos.RemoveAt(Ptr);
			GoalInfoList.MarkArrayDirty();
			return;
		}
		GoalInfoList.GoalInfos[Ptr].GoalDesc = QuestCommonNodePtr->GetGoalDesc();
		GoalInfoList.GoalInfos[Ptr].bChecked = QuestCommonNodePtr->bGoalFinished;
		GoalInfoList.MarkItemDirty(GoalInfoList.GoalInfos[Ptr]);
		OnRep_GoalInfoList();
	}
	else
	{
		auto NewGoalInfo = FGoalInfo(NodeTemplate, QuestCommonNodePtr->GetGoalDesc(), QuestCommonNodePtr->bGoalFinished);
		GoalInfoList.GoalInfos.Add(NewGoalInfo);
		GoalInfoList.MarkItemDirty(NewGoalInfo);
		OnRep_GoalInfoList();
	}
}

void UQuestGlobalComponent::AcceptQuest(UFlowAsset* QuestFlow)
{
	if (!IsQuestFlowValid(QuestFlow))
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

	QuestName = UFlowExtraFunctionLibrary::GetCurrentQuestInfo(SelectedQuestFlow)->QuestName;
	
	for (auto Itr : GoalInfoList.GoalInfos)
	{
		if (Itr.QuestCommonNodeDefault->GetFlowAsset() == SelectedQuestFlow)
		{
			Result.Add(Itr);
		}
	}
	QuestInfos = Result;
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
