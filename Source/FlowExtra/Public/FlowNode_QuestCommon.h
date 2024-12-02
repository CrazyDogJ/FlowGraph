// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/World/FlowNode_OnNotifyFromActor.h"
#include "FlowNode_QuestCommon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FLOWEXTRA_API UFlowNode_QuestCommon : public UFlowNode
{
	GENERATED_BODY()

public:
	UFlowNode_QuestCommon();

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

	UFUNCTION(BlueprintImplementableEvent, Category=Quest)
	FText GetGoalDesc();

	UFUNCTION(BlueprintCallable, Category=Quest)
	void MarkThisGoalDirty(bool bFinished);
	
	UFUNCTION(BlueprintImplementableEvent, Category=Quest)
	void OnNotify(UObject* Object1, UObject* Object2);
	
	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=OnReceived)
	void K2_OnEventReceived();
	
	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=OnCompleted)
    void K2_OnEventCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=HasAnyGoalActors)
	void K2_HasAnyGoalActors();

	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=HasNoGoalActors)
	void K2_HasNoGoalActors();
#if WITH_EDITOR
public:
	virtual FString GetNodeDescription() const override;
#endif
};
