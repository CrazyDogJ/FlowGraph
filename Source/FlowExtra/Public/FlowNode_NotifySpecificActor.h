// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "FlowComponent.h"
#include "FlowNode_NotifySpecificActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "NotifySpecActor"))
class FLOWEXTRA_API UFlowNode_NotifySpecificActor : public UFlowNode
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Notify")
	FGameplayTagContainer NotifyTags;

	// If true, node will check given Notify Tag is present in the Recently Sent Notify Tags
	// This might be helpful in multiplayer, if client-side Flow Node started work after server sent the notify
	UPROPERTY(EditAnywhere, Category = "Notify")
	bool bRetroactive;

	TWeakObjectPtr<UFlowComponent> FlowComponent;

protected:
	virtual void ExecuteInput(const FName& PinName) override;
	virtual void OnLoad_Implementation() override;

	virtual void StartObserving();
	virtual void StopObserving();

	virtual void ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) {}
	virtual void ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) {}

	virtual void OnNotifyFromComponent(UFlowComponent* Component, const FGameplayTag& Tag);

	UFUNCTION()
	virtual void OnEventReceived();

	virtual void Cleanup() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnReceived();
};
