// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FlowExtraSettings.generated.h"

class UDialogueWidget;

UCLASS(Config = Game, DefaultConfig, meta=(DisplayName="Flow Extra Settings"))
class FLOWEXTRA_API UFlowExtraSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetContainerName() const override { return TEXT("Project"); }
	virtual FName GetCategoryName() const override { return TEXT("Flow Graph"); }
	virtual FName GetSectionName() const override { return TEXT("Extra"); }

	/** Dialogue widget class. */
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;
};
