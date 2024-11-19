// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueCameraCalculation.h"
#include "FlowNode_NotifySpecificActor.h"
#include "FlowNode_Dialogue.generated.h"

USTRUCT(BlueprintType)
struct FDialogueAnimation
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, float> Morphs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bStopMontageOnEnd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bStopPreviousMontageOnStart;
};

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Dialogue"))
class FLOWEXTRA_API UFlowNode_Dialogue : public UFlowNode
{
	GENERATED_UCLASS_BODY()

public:
#pragma region Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow|Text", meta = (Multiline = "true"))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow|Text")
	TMap<FName, FText> Options;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flow|Camera", Instanced)
	TObjectPtr<UDialogueCameraCalculation> DialogueCameraCalculation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flow|Animation")
	TMap<FGameplayTag, FDialogueAnimation> DialogueAnimation;
#pragma endregion

#pragma region Functions
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Flow")
	void ContinueDialogue(int SelectionIndex);
	
	UFUNCTION(BlueprintPure, Category = "Flow")
	bool HasOptions() const;
#pragma endregion
	
#pragma region Override
	virtual void ExecuteInput(const FName& PinName) override;
	virtual void Finish() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual FString GetNodeDescription() const override;
#endif
#pragma endregion 
};
