// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestGlobalComponent.h"
#include "UObject/Object.h"
#include "QuestDelegate.generated.h"

class UFlowComponent_Quest;
class UFlowAsset_Quest;
class UFlowNode_QuestCommon;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class FLOWEXTRA_API UQuestDelegate : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UFlowComponent_Quest* OwnerQuestFlowComp;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UFlowAsset_Quest* ListeningQuest;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGuid> ListeningQuestGoals;

	UFUNCTION(BlueprintImplementableEvent)
	void OnQuestStart(UFlowAsset_Quest* QuestInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void OnQuestEnd(UFlowAsset_Quest* QuestInstance, EQuestFlowState QuestFlowState);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnQuestNodeStateChanged(UFlowNode_QuestCommon* NodeInstance, EGoalState GoalState);
};
