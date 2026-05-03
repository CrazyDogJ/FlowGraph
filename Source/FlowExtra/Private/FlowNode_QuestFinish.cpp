// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestFinish.h"

#include "FlowAsset.h"
#include "FlowComponent_Quest.h"
#include "FlowExtraGameplayTags.h"
#include "FlowSubsystem.h"
#include "QuestGlobalComponent.h"

UFlowNode_QuestFinish::UFlowNode_QuestFinish()
{
#if WITH_EDITOR
	Category = TEXT("Quest");
	NodeDisplayStyle = FlowNodeStyle::InOut;
#endif
	OutputPins = {};
	AllowedSignalModes = {EFlowSignalMode::Enabled, EFlowSignalMode::Disabled};
}

void UFlowNode_QuestFinish::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	// Quest delegates
	if (auto QuestFlow = Cast<UFlowAsset_Quest>(GetFlowAsset()))
	{
		auto Comps = GetFlowSubsystem()->GetFlowComponentsByTag(FlowQuestTags::FlowQuestComp, UFlowComponent_Quest::StaticClass(), false);
		for (auto Comp : Comps)
		{
			if (auto Comp_Quest = Cast<UFlowComponent_Quest>(Comp))
			{
				for (auto Delegate : Comp_Quest->QuestDelegates)
				{
					if (Delegate->ListeningQuest == QuestFlow->GetTemplateAsset())
					{
						Delegate->OnQuestEnd(bSuccessOrFailed ? QFS_Finished : QFS_Failed);
					}
				}
			}
		}
	}
	
	if (auto QuestComp = Cast<UQuestGlobalComponent>(GetFlowAsset()->GetOwner()))
	{
		const auto Found = QuestComp->QuestFlowStateList.QuestStates.IndexOfByPredicate([&](const FQuestFlowState& State)
		{
			return State.QuestFlowTemplate == GetFlowAsset()->GetTemplateAsset();
		});
		
		if (Found >= 0)
		{
			auto& FoundNodeState = QuestComp->QuestFlowStateList.QuestStates[Found];
			FoundNodeState.QuestFlowState = bSuccessOrFailed ? QFS_Finished : QFS_Failed;
			FoundNodeState.FinishNodeGuid = GetGuid();
			
			// iterate nodes and save it
			TArray<UFlowNode*> NodesInExecutionOrder;
			GetFlowAsset()->GetNodesInExecutionOrder<UFlowNode>(GetFlowAsset()->GetDefaultEntryNode(), NodesInExecutionOrder);
			for (UFlowNode* Node : NodesInExecutionOrder)
			{
				if (Node && Node->GetActivationState() == EFlowNodeState::Completed)
				{
					if (auto Goal = Cast<UFlowNode_QuestCommon>(Node))
					{
						FoundNodeState.Nodes.Add(FFinishedGoalState(Node->GetGuid(), Goal->GetGoalDesc(Goal->GetGoalData()), Goal->CurrentGoalState));
					}
				}
			}
			
			QuestComp->QuestFlowStateList.MarkItemDirty(FoundNodeState);
			QuestComp->QuestStateChangedEvent.Broadcast({Found});
			// QuestComp->OnRep_QuestFlowStateList();
		}
		
		QuestComp->OnQuestFinished(
			Cast<UFlowAsset_Quest>(GetFlowAsset()->GetTemplateAsset()),
			bSuccessOrFailed ? QFS_Finished : QFS_Failed);
	}

	Finish();
}
