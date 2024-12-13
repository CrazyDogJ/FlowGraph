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

	UFlowNode_QuestFinish();
	
	virtual void ExecuteInput(const FName& PinName) override;
	virtual bool CanFinishGraph() const override {return true;}
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bSuccessOrFailed = true;
};
