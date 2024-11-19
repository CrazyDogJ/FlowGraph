// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomSpringCamera.h"
#include "FlowComponent.h"
#include "FlowNode_Dialogue.h"
#include "DialogueComponent_Base.generated.h"

class UDialogueWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueNodeNotify, UFlowNode_Dialogue*, DialogueNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueFlowNotify, UFlowAsset*, DialogueFlow);

UENUM(BlueprintType)
enum EDialogMontageMode : uint8
{
	EDMM_Play = 0,
	EDMM_StopInput = 1,
	EDMM_StopCurrent = 2
};

UENUM(BlueprintType)
enum EDialogRole : uint8
{
	EDR_Player = 0,
	EDR_DialogueOwner = 1,
	EDR_Extra = 2
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLOWEXTRA_API UDialogueComponent_Base : public UFlowComponent
{
	GENERATED_BODY()

public:
	UDialogueComponent_Base();

#pragma region Events
	UPROPERTY(BlueprintAssignable)
	FDialogueNodeNotify OnDialogueNodeStart;

	UPROPERTY(BlueprintAssignable)
	FDialogueNodeNotify OnDialogueNodeEnd;

	UPROPERTY(BlueprintAssignable)
	FDialogueFlowNotify OnDialogueFlowStart;

	UPROPERTY(BlueprintAssignable)
	FDialogueFlowNotify OnDialogueFlowEnd;

	UFUNCTION()
	void OnDialogueNodeStartEvent(UFlowNode_Dialogue* DialogueNode);

	UFUNCTION()
	void OnDialogueNodeEndEvent(UFlowNode_Dialogue* DialogueNode);

	UFUNCTION()
	void OnDialogueFlowStartEvent(UFlowAsset* DialogueFlow);

	UFUNCTION()
	void OnDialogueFlowEndEvent(UFlowAsset* DialogueFlow);
#pragma endregion 

#pragma region Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UDialogueWidget> WidgetClass;

	UPROPERTY(BlueprintReadOnly)
	UDialogueWidget* DialogueWidget;
	
	UPROPERTY(BlueprintReadOnly)
	UPrimitiveComponent* ActorPrimitiveComponent;

	UPROPERTY(BlueprintReadOnly)
	FName ComponentSocket;

	UPROPERTY(BlueprintReadOnly, Replicated)
	UAnimMontage* CurrentDialogueMontage;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bInDialogue = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TEnumAsByte<EDialogRole> CurrentRole = EDR_Extra;

	UPROPERTY(BlueprintReadOnly)
	ACustomSpringCamera* CurrentCamera;
#pragma endregion

#pragma region Functions
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void CharacterPlayMontage(UAnimMontage* AnimMontage, EDialogMontageMode Mode);

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void CharacterSetMorphs(const TArray<FName>& MorphNames, const TArray<float>& MorphAlpha, bool bSetOrClear);
	
	UFUNCTION(BlueprintCallable)
	void SetupVariables(UPrimitiveComponent* InPrimitiveComponent, FName InComponentSocket);

	/**
	 * Used for NPC start dialogue ONLY!
	 * @param FlowAsset In dialogue flow asset
	 * @param InteractedCharacter In dialogue player interacted
	 */
	UFUNCTION(BlueprintCallable)
	void StartDialogue(UFlowAsset* FlowAsset, AActor* InteractedCharacter);

	bool FindRole(FGameplayTag InTag) const;
#pragma endregion 
};
