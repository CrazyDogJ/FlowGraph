// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlowExtraFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FLOWEXTRA_API UFlowExtraFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = Flow)
	static TArray<FFlowAssetSaveData> GetFlowSaveDataFromSaveGame(UFlowSaveGame* FlowSaveGame);

	UFUNCTION(BlueprintCallable, Category = Flow)
	static void SaveSpecificFlowAsset(UFlowSaveGame* SaveGame, UFlowAsset* FlowAsset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Flow)
	static UFlowAsset* GetTemplateAssetFromInstance(UFlowAsset* Instance);
};
