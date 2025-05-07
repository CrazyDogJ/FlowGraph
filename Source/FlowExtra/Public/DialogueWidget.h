// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueComponent_Base.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FLOWEXTRA_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Dialogue asset owner component. */
	UPROPERTY(BlueprintReadOnly)
	UDialogueComponent_Base* DialogueComponent;
};
