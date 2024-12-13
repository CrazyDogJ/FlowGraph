// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestFinish.h"

#include "FlowAsset.h"
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

	if (auto QuestComp = Cast<UQuestGlobalComponent>(GetFlowAsset()->GetOwner()))
	{
		auto Found = QuestComp->QuestFlowStateList.QuestStates.FindByPredicate([&](const FQuestFlowState& State)
		{
			return State.QuestFlowTemplate == GetFlowAsset()->GetTemplateAsset();
		});
		
		if (Found)
		{
			Found->QuestFlowState = bSuccessOrFailed ? QFS_Finished : QFS_Failed;
			Found->FinishNodeGuid = GetGuid();
			
			// iterate nodes and save it
			TArray<UFlowNode*> NodesInExecutionOrder;
			GetFlowAsset()->GetNodesInExecutionOrder<UFlowNode>(GetFlowAsset()->GetDefaultEntryNode(), NodesInExecutionOrder);
			for (UFlowNode* Node : NodesInExecutionOrder)
			{
				if (Node && Node->GetActivationState() == EFlowNodeState::Completed)
				{
					if (auto Goal = Cast<UFlowNode_QuestCommon>(Node))
					{
						Found->Nodes.Add(FFinishedGoalState(Goal->GetGoalDesc(), Goal->CurrentGoalState));
					}
				}
			}
			
			QuestComp->QuestFlowStateList.MarkItemDirty(*Found);
			QuestComp->OnRep_QuestFlowStateList();
		}
	}

	Finish();
}
