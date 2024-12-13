// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "FlowAsset_Quest.generated.h"

/**
 * Quest flow containing quest goal nodes.
 */
UCLASS(BlueprintType, hideCategories = Object)
class FLOWEXTRA_API UFlowAsset_Quest : public UFlowAsset
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString QuestID;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText QuestName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText QuestDescription;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	FFlowAssetSaveData SaveQuestInstance();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void LoadQuestInstance(const FFlowAssetSaveData& AssetRecord);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
