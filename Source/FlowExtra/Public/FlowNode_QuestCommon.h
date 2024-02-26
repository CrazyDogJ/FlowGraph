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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quest, meta=(EditCondition="!bUseStaticLocation"))
	FGameplayTagContainer MapPinActorIdentifyTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quest)
	bool bUseStaticLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quest, meta=(EditCondition="bUseStaticLocation"))
	TArray<FVector> StaticQuestGoalLocation;

	virtual void OnEventReceived() override;

	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=OnReceived)
	void K2_OnEventReceived();
	
	UFUNCTION(BlueprintImplementableEvent, Category=Quest, DisplayName=OnCompleted)
    void K2_OnEventCompleted();
	
	virtual void Cleanup() override;

	virtual void OnLoad_Implementation() override;
	
#if WITH_EDITOR
public:
	virtual FString GetNodeDescription() const override;
#endif
};
