// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestCommon.h"

void UFlowNode_QuestCommon::OnEventReceived()
{
	K2_OnEventReceived();

	SuccessCount++;
	if (SuccessLimit > 0 && SuccessCount == SuccessLimit)
	{
		K2_OnEventCompleted();
	}
}

void UFlowNode_QuestCommon::Cleanup()
{
	Super::Cleanup();

	K2_Cleanup();
}

void UFlowNode_QuestCommon::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();

	K2_OnActivate();
}

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
