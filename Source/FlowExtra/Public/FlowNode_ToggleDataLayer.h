// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "FlowNode_ToggleDataLayer.generated.h"

USTRUCT(BlueprintType)
struct FFlowDataLayerState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UDataLayerAsset> DataLayerAsset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDataLayerRuntimeState DataLayerState = EDataLayerRuntimeState::Activated;
};

/**
 * 
 */
UCLASS()
class FLOWEXTRA_API UFlowNode_ToggleDataLayer : public UFlowNode
{
	GENERATED_BODY()

	virtual void ExecuteInput(const FName& PinName) override;
	virtual FString GetNodeCategory() const override;
	virtual FString GetNodeDescription() const override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FFlowDataLayerState> DataLayerAssets;
};
