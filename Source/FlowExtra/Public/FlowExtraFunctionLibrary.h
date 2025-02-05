// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowNode_Dialogue.h"
#include "FlowNode_QuestCommon.h"
#include "FlowNode_QuestInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlowExtraFunctionLibrary.generated.h"

// Struct for rich text start
USTRUCT(BlueprintType)
struct FRichTextFlag
{
	GENERATED_USTRUCT_BODY()
 
	UPROPERTY(BlueprintReadOnly)
	int  StartIndex = -1;
 
	UPROPERTY(BlueprintReadOnly)
	int  EndIndex = -1;
 
	UPROPERTY(BlueprintReadOnly)
	FString FlagString;
};
 
USTRUCT(BlueprintType)
struct FRichTextContext
{
	GENERATED_USTRUCT_BODY()
 
	UPROPERTY(BlueprintReadOnly)
	FString SourceString;
 
	UPROPERTY(BlueprintReadOnly)
	FString PureString;
 
	UPROPERTY(BlueprintReadOnly)
	TArray<FRichTextFlag> FlagData;
};
// Struct for rich text end

/**
 * 
 */
UCLASS()
class FLOWEXTRA_API UFlowExtraFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = Flow)
	static TArray<FFlowAssetSaveData> GetFlowSaveDataFromSaveGame(UFlowSaveGame* FlowSaveGame);

	UFUNCTION(BlueprintCallable, Category = Flow)
	static void SaveSpecificFlowAsset(UFlowSaveGame* SaveGame, UFlowAsset* FlowAsset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Flow)
	static UFlowAsset* GetTemplateAssetFromInstance(UFlowAsset* Instance);

	UFUNCTION(BlueprintCallable, Category = Flow)
	static UFlowNode_Dialogue* GetCurrentDialogueNode(UObject* DialogueFlowOwner);
	
	UFUNCTION(BlueprintCallable, Category = Flow)
	static void NotifyDialogueNode(int Index, UObject* FlowOwner);

	UFUNCTION(BlueprintCallable, Category = Flow)
	static UFlowNode_QuestInfo* GetCurrentQuestInfo(UFlowAsset* FlowInstance);

	UFUNCTION(BlueprintCallable, Category = Flow)
	static void CompareArray(TArray<UFlowNode_QuestCommon*> ArrayA, TArray<UFlowNode_QuestCommon*> ArrayB,
	                         TArray<UFlowNode_QuestCommon*>& Add, TArray<UFlowNode_QuestCommon*>& Remove,
	                         TArray<UFlowNode_QuestCommon*>& Unchanged);

	UFUNCTION(BlueprintCallable, Category = "Flow|Dialogue")
	static void InitRichTextContext(const FString& RichText, FRichTextContext& RichTextContext);

	UFUNCTION(BlueprintCallable, Category = "Flow|Dialogue")
	static FString GetRichTextSubString(const FRichTextContext& RichTextContext, int PureCharLength);
};
