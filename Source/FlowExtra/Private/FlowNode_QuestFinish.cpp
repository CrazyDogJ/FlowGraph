// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestFinish.h"

#include "FlowAsset.h"
#include "QuestGlobalComponent.h"

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
			QuestComp->QuestFlowStateList.MarkItemDirty(*Found);
			QuestComp->OnRep_QuestFlowStateList();
		}
	}

	TriggerFirstOutput(true);
}
