// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestCommon.h"

#include "FlowAsset.h"
#include "QuestGlobalComponent.h"

UFlowNode_QuestCommon::UFlowNode_QuestCommon()
{
	InputPins = {FFlowPin(TEXT("Start")), FFlowPin(TEXT("Stop"))};
	OutputPins = {FFlowPin(TEXT("Success")), FFlowPin(TEXT("Completed")), FFlowPin(TEXT("Stopped"))};

}

void UFlowNode_QuestCommon::MarkThisGoalDirty(bool bFinished)
{
	auto QuestComp = Cast<UQuestGlobalComponent>(GetFlowAsset()->GetOwner());
	QuestComp->MarkGoalDirty(this, bFinished);
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