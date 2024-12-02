// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowNode_QuestCommon.h"
#include "FlowNode_QuestInfo.h"
#include "FlowAsset.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "QuestGlobalComponent.generated.h"

UENUM(BlueprintType)
enum EQuestFlowState : uint8
{
	QFS_Ongoing = 0,
	QFS_Finished = 1,
	QFS_Failed = 2,
	QFS_Invalid = 3
};

#pragma region QuestStates
/*
 * Used to show quests and states in ui
 */
USTRUCT(BlueprintType)
struct FQuestFlowState : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FQuestFlowState(): QuestFlowTemplate(nullptr), QuestFlowState(QFS_Invalid) {}
	FQuestFlowState(UFlowAsset* FlowAsset, EQuestFlowState Qfs_Ongoing)
		: QuestFlowTemplate(FlowAsset), QuestFlowState(Qfs_Ongoing) {}
	
	UPROPERTY(BlueprintReadOnly)
	UFlowAsset* QuestFlowTemplate;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EQuestFlowState> QuestFlowState;
};

USTRUCT(BlueprintType)
struct FQuestFlowStateList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FQuestFlowState, FQuestFlowStateList>(QuestStates, DeltaParms, *this);
	}
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FQuestFlowState> QuestStates;
};

template<>
struct TStructOpsTypeTraits<FQuestFlowStateList> : public TStructOpsTypeTraitsBase2<FQuestFlowStateList>
{
	enum { WithNetDeltaSerializer = true };
};
#pragma endregion

#pragma region Goals
USTRUCT(BlueprintType)
struct FGoalInfo : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGoalInfo() : QuestCommonNodeDefault(nullptr), bChecked(false) {}
	FGoalInfo(UFlowNode_QuestCommon* QuestCommonNodeDefault, FText InGoalDesc, bool bChecked)
		: QuestCommonNodeDefault(QuestCommonNodeDefault), GoalDesc(InGoalDesc), bChecked(bChecked) {}
	
	UPROPERTY(BlueprintReadOnly)
	UFlowNode_QuestCommon* QuestCommonNodeDefault;
	
	UPROPERTY(BlueprintReadOnly)
	FText GoalDesc;

	UPROPERTY(BlueprintReadOnly)
	bool bChecked;
};

USTRUCT(BlueprintType)
struct FGoalInfoList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FGoalInfo, FGoalInfoList>(GoalInfos, DeltaParms, *this);
	}
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FGoalInfo> GoalInfos;
};

template<>
struct TStructOpsTypeTraits<FGoalInfoList> : public TStructOpsTypeTraitsBase2<FGoalInfoList>
{
	enum { WithNetDeltaSerializer = true };
};
#pragma endregion

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRecordFlowChangedEvent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class FLOWEXTRA_API UQuestGlobalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestGlobalComponent();

#pragma region Functions
	
	bool IsQuestFlowValid(UFlowAsset* QuestFlowTemplate);

	void MarkGoalDirty(UFlowNode_QuestCommon* QuestCommonNodePtr, bool bNeedRemoved);
	
	/**
	 * Start a quest on server.
	 * @param QuestFlow Quest flow template
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Quest")
	void AcceptQuest(UFlowAsset* QuestFlow);

	/**
	 * Get current ongoing quest flow instances.
	 * @return Ongoing quest instances
	 */
	UFUNCTION(BlueprintCallable, Category="Quest")
	TArray<UFlowAsset*> GetOngoingQuestInstances();

	/**
	 * Get quest state
	 * @param FlowTemplate Input quest flow template, used to search in recorded flow states
	 * @return Quest state enum
	 */
	UFUNCTION(BlueprintPure, Category="Quest")
	EQuestFlowState GetQuestFlowState(const UFlowAsset* FlowTemplate);

	/**
	 * Locally set current selected quest.
	 * @param FlowAsset Input quest flow template
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Quest")
	void SetSelectedQuestFlow(UFlowAsset* FlowAsset);

	/**
	 * Helper function, used to update ui elements.
	 * @param QuestInfos Return quest flow infos(include description)
	 * @param QuestName Return quest flow
	 */
	UFUNCTION(BlueprintCallable, Category="Quest")
	void GetSelectedQuestFlowInfo(TArray<FGoalInfo>& QuestInfos, FText& QuestName);

	/**
	* Used to notify goal nodes in current ongoing quest flow instances. 
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Quest")
	void NotifyGoalNodes(TSubclassOf<UFlowNode_QuestCommon> QuestGoalClass, UObject* Object1, UObject* Object2);

	UFUNCTION()
	void OnRep_QuestFlowStateList();

	UFUNCTION()
	void OnRep_GoalInfoList();
#pragma endregion

#pragma region Properties
	UPROPERTY(BlueprintAssignable)
	FRecordFlowChangedEvent SelectedQuestFlowChanged;

	UPROPERTY(BlueprintAssignable)
	FRecordFlowChangedEvent RecordedQuestFlowsChanged;
	
	// Local selected template.
	UPROPERTY(BlueprintReadOnly)
	UFlowAsset* SelectedQuestFlow;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_QuestFlowStateList)
	FQuestFlowStateList QuestFlowStateList;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_GoalInfoList)
	FGoalInfoList GoalInfoList;
#pragma endregion 
};
