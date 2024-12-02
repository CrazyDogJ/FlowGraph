// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "FlowNode_QuestInfo.generated.h"

/**
 * 
 */
UCLASS()
class FLOWEXTRA_API UFlowNode_QuestInfo : public UFlowNode
{
	GENERATED_BODY()

public:
	virtual void ExecuteInput(const FName& PinName) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText QuestName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText QuestDescription;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	FText QuestFinishedDescription;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	FText QuestFailedDescription;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	FGameplayTagContainer QuestTags;
};
