// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "InstancedStruct.h"
#include "FlowNode_QuestCommon.generated.h"

UENUM(BlueprintType)
enum EGoalState : uint8
{
	EGS_Ongoing = 0		UMETA(DisplayName = "Goal Ongoing"),
	EGS_Success = 1		UMETA(DisplayName = "Goal Successed"),
	EGS_Failed = 2		UMETA(DisplayName = "Goal Failed"),
	EGS_Stopped = 3		UMETA(DisplayName = "Goal Stopped"),
};

/**
 * 
 */
UCLASS(Blueprintable)
class FLOWEXTRA_API UFlowNode_QuestCommon : public UFlowNode
{
	GENERATED_BODY()

public:
	UFlowNode_QuestCommon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quest)
	FGameplayTagContainer MapPinActorIdentifyTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quest)
	bool bUseStaticLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quest)
	TArray<FVector> StaticQuestGoalLocation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category=Quest, SaveGame)
	TEnumAsByte<EGoalState> CurrentGoalState = EGS_Ongoing;
	
	UFUNCTION(BlueprintImplementableEvent, Category=Quest)
	FText GetGoalDesc();

	UFUNCTION(BlueprintCallable, Category=Quest)
	void MarkThisGoalDirty(TEnumAsByte<EGoalState> GoalState);
	
	UFUNCTION(BlueprintImplementableEvent, Category=Quest)
	void OnNotify(FInstancedStruct Data);

	virtual void ExecuteInput(const FName& PinName) override;
	virtual void TriggerOutput(const FName PinName, const bool bFinish = false, const EFlowPinActivationType ActivationType = EFlowPinActivationType::Default) override;
	virtual void OnLoad_Implementation() override;
#if WITH_EDITOR
public:
	virtual FString GetNodeDescription() const override;
#endif
};
