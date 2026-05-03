// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestGlobalComponent.h"

#include "FlowComponent_Quest.h"
#include "FlowExtraFunctionLibrary.h"
#include "FlowExtraGameplayTags.h"
#include "FlowNode_QuestFinish.h"
#include "FlowSubsystem.h"
#include "Net/UnrealNetwork.h"

void FQuestFlowStateList::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	if (OuterQuestComponent)
	{
		TArray<int32> Result;
		Result.Append(AddedIndices);
		for (const auto Itr : AddedIndices)
		{
			QuestStatesMapping.Add(QuestStates[Itr].QuestFlowTemplate, Itr);
		}
		
		OuterQuestComponent->QuestStateAddedEvent.Broadcast(Result);
	}
}

void FQuestFlowStateList::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	if (OuterQuestComponent)
	{
		TArray<int32> Result;
		Result.Append(ChangedIndices);
		OuterQuestComponent->QuestStateChangedEvent.Broadcast(Result);
	}
}

void FGoalInfoList::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	if (OuterQuestComponent)
    {
    	TArray<int32> Result;
    	Result.Append(AddedIndices);
		for (const auto Itr : AddedIndices)
		{
			GoalInfosMapping.Add(GoalInfos[Itr].QuestCommonNodeDefault, Itr);
		}
		
    	OuterQuestComponent->GoalInfoAddedEvent.Broadcast(Result);
    }
}

void FGoalInfoList::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	if (OuterQuestComponent)
	{
		TArray<int32> Result;
		Result.Append(ChangedIndices);
		OuterQuestComponent->GoalInfoChangedEvent.Broadcast(Result);
	}
}

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
	DOREPLIFETIME(UQuestGlobalComponent, PersistentTagFlag)
}

void UQuestGlobalComponent::PostLoad()
{
	Super::PostLoad();

	QuestFlowStateList.OuterQuestComponent = this;
	GoalInfoList.OuterQuestComponent = this;
}

void UQuestGlobalComponent::MarkGoalDirty(UFlowNode_QuestCommon* QuestCommonNodePtr, TEnumAsByte<EGoalState> GoalState)
{
	// Valid check
	if (!QuestCommonNodePtr)
	{
		return;
	}

	const auto NodeTemplate = Cast<UFlowNode_QuestCommon>(
		QuestCommonNodePtr->GetFlowAsset()->GetTemplateAsset()->GetNode(QuestCommonNodePtr->GetGuid()));

	if (auto Ptr = GoalInfoList.GoalInfosMapping.Find(NodeTemplate))
	{
		GoalInfoList.GoalInfos[*Ptr].GoalReplicatedData = QuestCommonNodePtr->GetGoalData();
		GoalInfoList.GoalInfos[*Ptr].GoalState = GoalState;
		GoalInfoList.MarkItemDirty(GoalInfoList.GoalInfos[*Ptr]);
		GoalInfoChangedEvent.Broadcast({*Ptr});
	}
	else
	{
		auto NewGoalInfo = FGoalInfo(NodeTemplate, QuestCommonNodePtr->GetGoalData(), GoalState);
		const auto Index = GoalInfoList.GoalInfos.Add(NewGoalInfo);
		GoalInfoList.GoalInfosMapping.Add(NodeTemplate, Index);
		GoalInfoList.MarkItemDirty(NewGoalInfo);
		GoalInfoAddedEvent.Broadcast({Index});
	}
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
		const auto Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>();
		if (Subsystem->StartRootFlow(this, QuestFlow, false))
		{
			auto NewItem = FQuestFlowState(QuestFlow, QFS_Ongoing);
			const auto Index = QuestFlowStateList.QuestStates.Add(NewItem);
			QuestFlowStateList.MarkItemDirty(NewItem);
			QuestFlowStateList.QuestStatesMapping.Add(QuestFlow, Index);
			QuestStateAddedEvent.Broadcast({Index});
			// OnRep_QuestFlowStateList();
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

EQuestFlowState UQuestGlobalComponent::GetQuestFlowState(const UFlowAsset* FlowTemplate) const
{
	if (!FlowTemplate)
	{
		return QFS_Invalid;
	}

	if (const auto Found = QuestFlowStateList.QuestStatesMapping.Find(FlowTemplate))
	{
		return QuestFlowStateList.QuestStates[*Found].QuestFlowState;
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

void UQuestGlobalComponent::NotifyGoalNodes(TSubclassOf<UFlowNode_QuestCommon> QuestGoalClass, FInstancedStruct Data)
{
	for (const auto Flow : GetOngoingQuestInstances())
	{
		const auto ActiveNodes = Flow->GetActiveNodes();
		for (auto ActiveGoalNode : ActiveNodes)
		{
			if (ActiveGoalNode->GetClass()->IsChildOf(QuestGoalClass))
			{
				Cast<UFlowNode_QuestCommon>(ActiveGoalNode)->OnNotify(Data);
			}
		}
	}
}

void UQuestGlobalComponent::NotifyPersistentTagFlag(FGameplayTag Tag, bool bAddOrNot)
{
	if (bAddOrNot)
	{
		PersistentTagFlag.AddTag(Tag);
	}
	else
	{
		PersistentTagFlag.RemoveTag(Tag);
	}
}

bool UQuestGlobalComponent::HasPersistentTagFlag(FGameplayTag Tag) const
{
	return PersistentTagFlag.HasTag(Tag);
}

void UQuestGlobalComponent::GetGoalState(FGuid DefaultNodeId, TEnumAsByte<EGoalState>& GoalState) const
{
	// Find in ongoing quest flow.
	const auto Found = GoalInfoList.GoalInfos.FindByPredicate([DefaultNodeId](const FGoalInfo& Info)
	{
		return Info.QuestCommonNodeDefault->GetGuid() == DefaultNodeId;
	});

	if (Found)
	{
		GoalState = Found->GoalState;
		return;
	}

	// Find in finished quest flow.
	for (const auto QuestState : QuestFlowStateList.QuestStates)
	{
		const auto FoundGoalState = QuestState.Nodes.FindByPredicate([DefaultNodeId](const FFinishedGoalState& FinishedGoalState)
		{
			return FinishedGoalState.GoalNodeGuid == DefaultNodeId;
		});
		
		if (FoundGoalState)
		{
			GoalState = FoundGoalState->GoalState;
			return;
		}
	}
	
	GoalState = EGS_NotFound;
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
	Result.TagContainer = PersistentTagFlag;
	Result.bValid = Result.OngoingQuestFlowAssetSaveData.Num() > 0 || Result.FinishedQuestFlowAssetSaveData.Num() > 0 || !Result.TagContainer.IsEmpty();
	return Result;
}

bool UQuestGlobalComponent::LoadQuestSaveData(FQuestSaveData SaveData)
{
	if (!SaveData.bValid)
	{
		return false;
	}

	PersistentTagFlag = SaveData.TagContainer;
	
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
		const auto Index = QuestFlowStateList.QuestStates.Add(NewState);
		QuestFlowStateList.MarkItemDirty(NewState);
		QuestFlowStateList.QuestStatesMapping.Add(Pair.Key, Index);
		QuestStateAddedEvent.Broadcast({Index});
	}
	for (auto Pair : SaveData.FinishedQuestFlowAssetSaveData)
	{
		auto NewState = FQuestFlowState(Pair.Key,
			Pair.Value.State,
			Pair.Value.FinishNodeGuids,
			Pair.Value.Nodes);
		const auto Index = QuestFlowStateList.QuestStates.Add(NewState);
		QuestFlowStateList.MarkItemDirty(NewState);
		QuestFlowStateList.QuestStatesMapping.Add(Pair.Key, Index);
		QuestStateAddedEvent.Broadcast({Index});
	}
	// OnRep_QuestFlowStateList();

	// When loaded, we call events for delegates.
	const auto Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>();
	auto Comps = Subsystem->GetFlowComponentsByTag(FlowQuestTags::FlowQuestComp, UFlowComponent_Quest::StaticClass(), false);
	for (auto Comp : Comps)
	{
		if (auto Comp_Quest = Cast<UFlowComponent_Quest>(Comp))
		{
			Comp_Quest->CallEvents();
		}
	}
	
	return true;
}

//void UQuestGlobalComponent::OnRep_QuestFlowStateList()
//{
//	if (QuestFlowStateList.QuestStates.Last().QuestFlowState == QFS_Ongoing)
//	{
//		SetSelectedQuestFlow(QuestFlowStateList.QuestStates.Last().QuestFlowTemplate);
//	}
//	RecordedQuestFlowsChanged.Broadcast();
//}

//void UQuestGlobalComponent::OnRep_GoalInfoList()
//{
//	SelectedQuestFlowChanged.Broadcast();
//}
