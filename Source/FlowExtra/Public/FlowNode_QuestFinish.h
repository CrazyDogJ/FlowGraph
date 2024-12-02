// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "FlowNode_QuestFinish.generated.h"

/**
 * 
 */
UCLASS()
class FLOWEXTRA_API UFlowNode_QuestFinish : public UFlowNode
{
	GENERATED_BODY()
	
	virtual void ExecuteInput(const FName& PinName) override;

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bSuccessOrFailed = true;
};
