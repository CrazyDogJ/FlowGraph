// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomSpringCamera.h"
#include "FlowAsset_Dialogue.h"
#include "FlowNode_Dialogue.h"
#include "DialogueComponent_Base.generated.h"

class UDialogueWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueNodeNotify, UFlowNode_Dialogue*, DialogueNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueFlowNotify, UFlowAsset_Dialogue*, DialogueFlow);

UENUM(BlueprintType)
enum EDialogMontageMode : uint8
{
	EDMM_Play = 0,
	EDMM_StopInput = 1,
	EDMM_StopCurrent = 2
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class FLOWEXTRA_API UDialogueComponent_Base : public UActorComponent
{
	GENERATED_BODY()

public:
	UDialogueComponent_Base();

#pragma region Events
	UPROPERTY(BlueprintAssignable)
	FDialogueNodeNotify OnDialogueNodeStart;

	UPROPERTY(BlueprintAssignable)
	FDialogueNodeNotify OnDialogueNodeTextChanged;
	
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
	void OnDialogueFlowStartEvent(UFlowAsset_Dialogue* DialogueFlow);

	UFUNCTION()
	void OnDialogueFlowEndEvent(UFlowAsset_Dialogue* DialogueFlow);
#pragma endregion 

#pragma region Properties
	UPROPERTY(BlueprintReadOnly)
	UPrimitiveComponent* ActorPrimitiveComponent;

	UPROPERTY(BlueprintReadOnly)
	FName ComponentSocket;

	UPROPERTY(BlueprintReadOnly, Replicated)
	UAnimMontage* CurrentDialogueMontage;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bInDialogue = false;

	UPROPERTY(BlueprintReadOnly)
	UFlowAsset_Dialogue* CurrentDialogueInstance;

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
	void StartDialogue(UFlowAsset_Dialogue* FlowAsset, AActor* InteractedCharacter);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDialogueWidget(TSubclassOf<UDialogueWidget> DialogueWidgetClass, UDialogueComponent_Base* DialogueComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void HideDialogueWidget();
	
	bool FindRole(const FGameplayTag& InTag) const;
#pragma endregion

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
