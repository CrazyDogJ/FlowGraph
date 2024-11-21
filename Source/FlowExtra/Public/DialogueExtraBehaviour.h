// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "DialogueExtraBehaviour.generated.h"

class UFlowNode_Dialogue;

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType)
class FLOWEXTRA_API UDialogueExtraBehaviour : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDialogueNodeStart(UFlowNode_Dialogue* DialogueNode);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDialogueNodeEnd(UFlowNode_Dialogue* DialogueNode);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer GameplayTags;
};
