// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "FlowNode_Dialogue_End.generated.h"

/**
 * 
 */
UCLASS()
class FLOWEXTRA_API UFlowNode_Dialogue_End : public UFlowNode
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteInput(const FName& PinName) override;
};
