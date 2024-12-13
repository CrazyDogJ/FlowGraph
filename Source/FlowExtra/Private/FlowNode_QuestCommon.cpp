// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestCommon.h"

#include "FlowAsset.h"
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