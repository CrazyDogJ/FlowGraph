// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/World/FlowNode_OnNotifyFromActor.h"
#include "FlowNode_QuestCommon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FLOWEXTRA_API UFlowNode_QuestCommon : public UFlowNode_OnNotifyFromActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quest)
	FGameplayTagContainer MapPinActorIdentifyTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quest)
	bool bUseStaticLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quest)
	TArray<FVector> StaticQuestGoalLocation;

	UPROPERTY(BlueprintReadWrite, Category=Quest)
	bool bGoalActivated;

	UPROPERTY(BlueprintReadWrite, Category=Quest)
	bool bGoalFinished;

	virtual void ExecuteInput(const FName& PinName) override;
	
	virtual void OnEventReceived() override;

	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=OnReceived)
	void K2_OnEventReceived();
	
	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=OnCompleted)
    void K2_OnEventCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=HasAnyGoalActors)
	void K2_HasAnyGoalActors();

	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=HasNoGoalActors)
	void K2_HasNoGoalActors();
	
	virtual void Cleanup() override;

	virtual void OnLoad_Implementation() override;

	virtual void ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;
	virtual void ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Notify")
	FGameplayTagContainer GetNotifyTags();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Notify")
	FGameplayTagContainer GetIdentityTags();
	
#if WITH_EDITOR
public:
	virtual FString GetNodeDescription() const override;
#endif
};
