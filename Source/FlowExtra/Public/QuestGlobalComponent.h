// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowNode_QuestCommon.h"
#include "FlowNode_QuestInfo.h"
#include "FlowAsset_Quest.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "QuestGlobalComponent.generated.h"

UENUM(BlueprintType)
enum EQuestFlowState : uint8
{
	QFS_Ongoing = 0		UMETA(DisplayName = "Quest Flow Ongoing"),
	QFS_Finished = 1	UMETA(DisplayName = "Quest Flow Finished"),
	QFS_Failed = 2		UMETA(DisplayName = "Quest Flow Failed"),
	QFS_Invalid = 3		UMETA(DisplayName = "Quest Flow Invalid"),
};

USTRUCT(BlueprintType)
struct FFinishedGoalState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FText GoalDesc;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EGoalState> GoalState;
};

USTRUCT(BlueprintType)
struct FFinishedQuestState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	TEnumAsByte<EQuestFlowState> State;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	FGuid FinishNodeGuids;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	TArray<FFinishedGoalState> Nodes;
};

USTRUCT(BlueprintType)
struct FQuestSaveData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	TMap<UFlowAsset*, FFinishedQuestState> FinishedQuestFlowAssetSaveData;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	TMap<UFlowAsset*, FFlowAssetSaveData> OngoingQuestFlowAssetSaveData;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	bool bValid = false;
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
	FQuestFlowState(UFlowAsset* FlowAsset, EQuestFlowState Qfs_Ongoing, FGuid FinishNodeGuid)
		: QuestFlowTemplate(FlowAsset), QuestFlowState(Qfs_Ongoing), FinishNodeGuid(FinishNodeGuid){}
	FQuestFlowState(UFlowAsset* FlowAsset, EQuestFlowState Qfs_Ongoing, FGuid FinishNodeGuid, TArray<FFinishedGoalState> Nodes)
		: QuestFlowTemplate(FlowAsset), QuestFlowState(Qfs_Ongoing), FinishNodeGuid(FinishNodeGuid), Nodes(Nodes){}
	
	UPROPERTY(BlueprintReadOnly)
	UFlowAsset* QuestFlowTemplate;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EQuestFlowState> QuestFlowState;

	UPROPERTY(BlueprintReadOnly)
	FGuid FinishNodeGuid;

	UPROPERTY(BlueprintReadOnly)
	TArray<FFinishedGoalState> Nodes;
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

	FGoalInfo() : QuestCommonNodeDefault(nullptr), GoalState(EGS_Ongoing) {}
	FGoalInfo(UFlowNode_QuestCommon* QuestCommonNodeDefault, const FText& InGoalDesc, EGoalState GoalState)
		: QuestCommonNodeDefault(QuestCommonNodeDefault), GoalDesc(InGoalDesc), GoalState(GoalState) {}
	
	UPROPERTY(BlueprintReadOnly)
	UFlowNode_QuestCommon* QuestCommonNodeDefault;
	
	UPROPERTY(BlueprintReadOnly)
	FText GoalDesc;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EGoalState> GoalState;
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
	void MarkGoalDirty(UFlowNode_QuestCommon* QuestCommonNodePtr, TEnumAsByte<EGoalState> GoalState);
	
	/**
	 * Start a quest on server.
	 * @param QuestFlow Quest flow template
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Quest")
	void AcceptQuest(UFlowAsset_Quest* QuestFlow);

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

	UFUNCTION(BlueprintCallable, Category="Quest")
	void GetSelectedFinishedQuestFlow(TArray<FFinishedGoalState>& QuestGoals);
	
	/**
	* Used to notify goal nodes in current ongoing quest flow instances. 
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Quest")
	void NotifyGoalNodes(TSubclassOf<UFlowNode_QuestCommon> QuestGoalClass, UObject* Object1, UObject* Object2);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Quest")
	FQuestSaveData GetQuestSaveData();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Quest")
	bool LoadQuestSaveData(FQuestSaveData SaveData);
	
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
