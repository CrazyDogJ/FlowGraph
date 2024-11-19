// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DialogueCameraCalculation.generated.h"

class UFlowNode_Dialogue;

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract, Blueprintable, BlueprintType)
class FLOWEXTRA_API UDialogueCameraCalculation : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UFlowNode_Dialogue* InstanceDialogueNode;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGameplayTag> IdentityTags;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CalculateCamera(FVector& Location, FRotator& Rotation, float& FOV, FVector& PivotLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
	TArray<AActor*> GetIgnoreActors();

	UFUNCTION(BlueprintImplementableEvent)
	void SetupVariables();

	UFUNCTION(BlueprintImplementableEvent)
	void ClearCamera();
};
