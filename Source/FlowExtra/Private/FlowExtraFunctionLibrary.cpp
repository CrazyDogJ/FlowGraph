// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowExtraFunctionLibrary.h"

#include "FlowAsset.h"
#include "FlowAsset_Dialogue.h"
#include "FlowNode_Dialogue.h"
#include "FlowSave.h"
#include "FlowSubsystem.h"

TArray<FFlowAssetSaveData> UFlowExtraFunctionLibrary::GetFlowSaveDataFromSaveGame(UFlowSaveGame* FlowSaveGame)
{
	return FlowSaveGame->FlowInstances;
}

void UFlowExtraFunctionLibrary::SaveSpecificFlowAsset(UFlowSaveGame* SaveGame, UFlowAsset* FlowAsset)
{
	if (SaveGame && FlowAsset)
	{
		FlowAsset->SaveInstance(SaveGame->FlowInstances);
	}
}

UFlowAsset* UFlowExtraFunctionLibrary::GetTemplateAssetFromInstance(UFlowAsset* Instance)
{
	if (Instance)
	{
		return Instance->GetTemplateAsset();
	}
	return nullptr;
}

UFlowNode_Dialogue* UFlowExtraFunctionLibrary::GetCurrentDialogueNode(UObject* DialogueFlowOwner)
{
	if (!DialogueFlowOwner)
	{
		return nullptr;
	}
	auto FlowSubsystem = DialogueFlowOwner->GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>();
	if (!FlowSubsystem)
	{
		return nullptr;
	}
	auto Instances = FlowSubsystem->GetRootInstancesByOwner(DialogueFlowOwner).Array();
	if (!Instances.IsValidIndex(0))
	{
		return nullptr;
	}
	if (auto CurrentDialogueFlow = Cast<UFlowAsset_Dialogue>(Instances[0]))
	{
		return CurrentDialogueFlow->CurrentDialogueNode;
	}

	return nullptr;
}

void UFlowExtraFunctionLibrary::NotifyDialogueNode(int Index, UObject* FlowOwner)
{
	if (FlowOwner)
	{
		if (auto Node = GetCurrentDialogueNode(FlowOwner))
		{
			Node->ContinueDialogue(Index);
		}
	}
}

UFlowNode_QuestInfo* UFlowExtraFunctionLibrary::GetCurrentQuestInfo(UFlowAsset* FlowInstance)
{
	if (FlowInstance)
	{
		for (auto Node : FlowInstance->GetNodes())
		{
			if (auto FindNode = Cast<UFlowNode_QuestInfo>(Node.Value))
			{
				return FindNode;
			}
		}
	}

	return nullptr;
}

void UFlowExtraFunctionLibrary::CompareArray(TArray<UFlowNode_QuestCommon*> ArrayA, TArray<UFlowNode_QuestCommon*> ArrayB,
							 TArray<UFlowNode_QuestCommon*>& Add, TArray<UFlowNode_QuestCommon*>& Remove,
							 TArray<UFlowNode_QuestCommon*>& Unchanged)
{
	TArray<UFlowNode_QuestCommon*> SortedA = ArrayA;
	TArray<UFlowNode_QuestCommon*> SortedB = ArrayB;
	Algo::Sort(SortedA);
	Algo::Sort(SortedB);
	
	int32 IndexA = 0, IndexB = 0;
	while (IndexA < SortedA.Num() && IndexB < SortedB.Num())
	{
		if (SortedA[IndexA] < SortedB[IndexB])
		{
			Remove.Add(SortedA[IndexA]);
			IndexA++;
		}
		else if (SortedA[IndexA] > SortedB[IndexB])
		{
			Add.Add(SortedB[IndexB]);
			IndexB++;
		}
		else
		{
			Unchanged.Add(SortedA[IndexA]);
			IndexA++;
			IndexB++;
		}
	}

	// 处理剩余的元素
	for (; IndexA < SortedA.Num(); IndexA++)
	{
		Remove.Add(SortedA[IndexA]);
	}
	for (; IndexB < SortedB.Num(); IndexB++)
	{
		Add.Add(SortedB[IndexB]);
	}
}

//Rich TEXT
void UFlowExtraFunctionLibrary::InitRichTextContext(const FString& RichText, FRichTextContext& RichTextContext)
{
	// 我早说过，这个<yellow>古董</>是<red>赝品</>啦！
	RichTextContext.SourceString = RichText;
	RichTextContext.PureString.Empty();
	RichTextContext.FlagData.Empty();
 
	bool SeekFlagStart = false;
	bool SeekFlagEnd = false;
	FRichTextFlag TempFlag;
	FString TempString;
	for (int i = 0; i < RichText.Len();)
	{
		auto CurrentChar = RichText[i];
		if (SeekFlagStart)
		{
			TempString.AppendChar(CurrentChar);
			if (CurrentChar == '>')
			{//seek start over
				SeekFlagStart = false;
				SeekFlagEnd = true;
				TempFlag.FlagString = TempString;
				TempFlag.StartIndex = RichTextContext.PureString.Len();
			}
		}
		else if (SeekFlagEnd)
		{
			if (CurrentChar == '<')
			{//找到尾部标识
				SeekFlagEnd = false;
				TempFlag.EndIndex = RichTextContext.PureString.Len();
				RichTextContext.FlagData.Add(TempFlag);
				i += 2;
			}
			else
			{
				RichTextContext.PureString.AppendChar(CurrentChar);
			}
		}
		else
		{
			if (CurrentChar == '<')
			{
				SeekFlagStart = true;
				TempString.Empty();
				TempString.AppendChar(CurrentChar);
			}
			else
				RichTextContext.PureString.AppendChar(CurrentChar);
		}
		++i;
	}
}
 
FString UFlowExtraFunctionLibrary::GetRichTextSubString(const FRichTextContext& RichTextContext, int PureCharLength)
{
	FString SubString = RichTextContext.PureString.Left(PureCharLength);
 
	int InsertIndex = -1;
	for (int i = 0; i < RichTextContext.FlagData.Num(); ++i)
	{
		if (RichTextContext.FlagData[i].StartIndex < PureCharLength)
			InsertIndex++;
		else
			break;
	}
 
	for (int i = RichTextContext.FlagData.Num() - 1; i >= 0; --i)
	{
		const auto& Data = RichTextContext.FlagData[i];
		if (i <= InsertIndex)
		{
			if (PureCharLength >= Data.EndIndex)
				SubString.InsertAt(Data.EndIndex, TEXT("</>"));
			else
				SubString.Append(TEXT("</>"));
			SubString.InsertAt(Data.StartIndex, Data.FlagString);
		}
	}
 
	return SubString;
}