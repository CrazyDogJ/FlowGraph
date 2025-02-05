// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "FlowAsset_Dialogue.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, hideCategories = Object)
class FLOWEXTRA_API UFlowAsset_Dialogue : public UFlowAsset
{
	GENERATED_BODY()

	virtual void FinishFlow(const EFlowFinishPolicy InFinishPolicy, const bool bRemoveInstance) override;
	
public:
	/**
	 * Tags to identify near dialogue objects;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FGameplayTag> IdentityTags;

	/**
	 * Radius to check near dialogue objects;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	float Radius = 500.0f;

	/**
	 * Center actor;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FGameplayTag CenterActorTag = FGameplayTag::RequestGameplayTag(TEXT("Flow.DialogInvolver.Player"));
	
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TMap<FGameplayTag, TObjectPtr<AActor>> IdentityActors;

	UPROPERTY(BlueprintReadOnly, Category = "Flow|Extra")
	UFlowNode_Dialogue* CurrentDialogueNode;
	
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetIdentityActors();

	UFUNCTION(BlueprintCallable)
	void UpdateNearNPC();

	UFUNCTION(BlueprintCallable)
	void SetupVariables(AActor* Player, AActor* DialogueObject);
};
