// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_NotifySpecificActor.h"

#include "FlowAsset.h"

UFlowNode_NotifySpecificActor::UFlowNode_NotifySpecificActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bRetroactive(false)
{
#if WITH_EDITOR
	NodeStyle = EFlowNodeStyle::Condition;
	Category = TEXT("World");
#endif

	InputPins = {FFlowPin(TEXT("Start")), FFlowPin(TEXT("Stop"))};
	OutputPins = {FFlowPin(TEXT("Success")), FFlowPin(TEXT("Completed")), FFlowPin(TEXT("Stopped"))};
}

void UFlowNode_NotifySpecificActor::ExecuteInput(const FName& PinName)
{
	if (PinName == TEXT("Start"))
	{
		StartObserving();
	}
	else if (PinName == TEXT("Stop"))
	{
		TriggerOutput(TEXT("Stopped"), true);
	}
	K2_ExecuteInput(PinName);
}

void UFlowNode_NotifySpecificActor::StartObserving()
{
	FlowComponent = Cast<UFlowComponent>(Cast<UActorComponent>(GetFlowAsset()->GetOwner())->GetOwner()->GetComponentByClass(UFlowComponent::StaticClass()));
	if (FlowComponent != nullptr)
	{
		FlowComponent->OnNotifyFromComponent.AddUObject(this, &UFlowNode_NotifySpecificActor::OnNotifyFromComponent);
		if (bRetroactive && FlowComponent->GetRecentlySentNotifyTags().HasAnyExact(NotifyTags))
		{
			OnEventReceived();
		}
	}
}

void UFlowNode_NotifySpecificActor::StopObserving()
{
	if (FlowComponent != nullptr)
	{
		FlowComponent->OnNotifyFromComponent.RemoveAll(this);
	}
}

void UFlowNode_NotifySpecificActor::OnNotifyFromComponent(UFlowComponent* Component, const FGameplayTag& Tag)
{
	if ((!NotifyTags.IsValid() || NotifyTags.HasTagExact(Tag)))
	{
		OnEventReceived();
	}
}

void UFlowNode_NotifySpecificActor::OnEventReceived()
{
	OnReceived();
}

void UFlowNode_NotifySpecificActor::Cleanup()
{
	StopObserving();
}

void UFlowNode_NotifySpecificActor::OnLoad_Implementation()
{
	StartObserving();
}