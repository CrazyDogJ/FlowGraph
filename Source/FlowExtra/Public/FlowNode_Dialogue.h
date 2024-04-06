// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowNode_NotifySpecificActor.h"
#include "FlowNode_Dialogue.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Dialogue"))
class FLOWEXTRA_API UFlowNode_Dialogue : public UFlowNode_NotifySpecificActor
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow", meta = (Multiline = "true"))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TMap<FName, FText> Options;

	UFUNCTION(BlueprintPure, Category = "Flow")
	bool HasOptions() const;
	
	virtual void OnLoad_Implementation() override;

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual FString GetNodeDescription() const override;
#endif
};
