// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestCommon.h"

void UFlowNode_QuestCommon::ExecuteInput(const FName& PinName)
{
	if (IdentityTags.IsValid())
	{
		if (PinName == TEXT("Start"))
		{
			StartObserving();
			bGoalActivated = true;
		}
		else if (PinName == TEXT("Stop"))
		{
			bGoalActivated = false;
		}
		K2_ExecuteInput(PinName);
	}
	else
	{
		LogError(MissingIdentityTag);
	}
}

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

void UFlowNode_QuestCommon::ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	bool bContain = true;
	if (!RegisteredActors.Contains(Actor))
	{
		bContain = false;
	}
	
	Super::ObserveActor(Actor, Component);

	if (RegisteredActors.Contains(Actor) && !bContain)
	{
		K2_HasAnyGoalActors();
	}
}

void UFlowNode_QuestCommon::ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component)
{
	Super::ForgetActor(Actor, Component);
	
	if (!RegisteredActors.Contains(Actor))
	{
		K2_HasNoGoalActors();
	}
}

FGameplayTagContainer UFlowNode_QuestCommon::GetNotifyTags()
{
	return NotifyTags;
}

FGameplayTagContainer UFlowNode_QuestCommon::GetIdentityTags()
{
	return IdentityTags;
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