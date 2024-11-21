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

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bSpawnNewCamera = true;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CalculateCamera(float DeltaTime, FVector& Location, FRotator& Rotation, float& FOV, FVector& PivotLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FPostProcessSettings CalculateCameraPostProcess(float DeltaTime, float& BlendAlpha);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetViewActor();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
	TArray<AActor*> GetIgnoreActors();

	UFUNCTION(BlueprintImplementableEvent)
	void SetupVariables();

	UFUNCTION(BlueprintImplementableEvent)
	void ClearCamera();
};
