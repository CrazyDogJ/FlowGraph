// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestCommon.h"

#include "FlowAsset.h"
#include "FlowComponent_Quest.h"
#include "FlowExtraGameplayTags.h"
#include "FlowSubsystem.h"
#include "QuestGlobalComponent.h"

UFlowNode_QuestCommon::UFlowNode_QuestCommon()
{
	InputPins = {FFlowPin(TEXT("Start")), FFlowPin(TEXT("Stop"))};
	OutputPins = {FFlowPin(TEXT("Success")), FFlowPin(TEXT("Completed")), FFlowPin(TEXT("Stopped"))};
#if WITH_EDITOR
	Category = TEXT("Quest");
#endif
}

void UFlowNode_QuestCommon::MarkThisGoalDirty(TEnumAsByte<EGoalState> GoalState)
{
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
					if (Delegate->ListeningQuest == QuestFlow->GetTemplateAsset() && Delegate->ListeningQuestGoals.Find(NodeGuid) >= 0)
					{
						Delegate->OnQuestNodeStateChanged(this, GoalState);
					}
				}
			}
		}
	}
	
	CurrentGoalState = GoalState;
	auto QuestComp = Cast<UQuestGlobalComponent>(GetFlowAsset()->GetOwner());
	QuestComp->MarkGoalDirty(this, GoalState);
}

void UFlowNode_QuestCommon::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	
	if (PinName == TEXT("Start"))
	{
		MarkThisGoalDirty(EGS_Ongoing);
	}
	else if (PinName == TEXT("Stop"))
	{
		MarkThisGoalDirty(EGS_Stopped);
		TriggerOutput(TEXT("Stopped"), true);
	}
}

void UFlowNode_QuestCommon::TriggerOutput(const FName PinName, const bool bFinish,
	const EFlowPinActivationType ActivationType)
{
	// Also mark goal dirty.
	if (PinName == TEXT("Success"))
	{
		MarkThisGoalDirty(EGS_Success);
	}
	else if (PinName == TEXT("Completed"))
	{
		MarkThisGoalDirty(EGS_Failed);
	}
	
	Super::TriggerOutput(PinName, bFinish, ActivationType);
}

void UFlowNode_QuestCommon::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();

	MarkThisGoalDirty(CurrentGoalState);
}

#if WITH_EDITOR
FString UFlowNode_QuestCommon::GetNodeDescription() const
{
	FString MapPinInfo;
	if (bUseStaticLocation)
	{
		MapPinInfo = "MapPinLocation : ";
		for (auto location : StaticQuestGoalLocation)
		{
			MapPinInfo += location.ToString() + LINE_TERMINATOR;
		}
	}
	else
	{
		MapPinInfo = "MapPinTag : " + MapPinActorIdentifyTags.ToString();
	}
	return Super::GetNodeDescription() + LINE_TERMINATOR + MapPinInfo + LINE_TERMINATOR + K2_GetNodeDescription();
}
#endif