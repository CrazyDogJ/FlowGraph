// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "Nodes/FlowNode.h"
#include "FlowNode_Dialogue_Start.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, DisplayName = "Dialogue Start")
class FLOWEXTRA_API UFlowNode_Dialogue_Start : public UFlowNode
{
	GENERATED_BODY()

public:
	UFlowNode_Dialogue_Start();

	/**
	 * Tags to identify near dialogue objects;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FGameplayTag> IdentityTags;

	/**
	 * Radius to check near dialogue objects;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Radius = 500.0f;

	/**
	 * Center actor;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag CenterActorTag = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Flow.DialogInvolver.Player"));;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, TObjectPtr<AActor>> IdentityActors;

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetIdentityActors();

	UFUNCTION(BlueprintCallable)
	void UpdateNearNPC();

	UFUNCTION(BlueprintCallable)
	void SetupVariables(AActor* Player, AActor* DialogueObject);

	// Override
	virtual FString GetNodeDescription() const override;
};
