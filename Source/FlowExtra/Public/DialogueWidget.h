// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

class DialogueComponent_Base;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FLOWEXTRA_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UDialogueComponent_Base* DialogueComponent;
};
