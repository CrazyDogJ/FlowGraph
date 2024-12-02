// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowExtraFunctionLibrary.h"

#include "FlowAsset.h"
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
	auto ActiveNodes = Instances[0]->GetActiveNodes();
	if (ActiveNodes.IsValidIndex(1))
	{
		return Cast<UFlowNode_Dialogue>(ActiveNodes[1]);
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

UFlowNode_Dialogue_Start* UFlowExtraFunctionLibrary::GetCurrentDialogueInfos(UFlowAsset* FlowInstance)
{
	if (FlowInstance)
	{
		if (FlowInstance->GetActiveNodes().IsValidIndex(0))
		{
			if (auto Info = Cast<UFlowNode_Dialogue_Start>(FlowInstance->GetActiveNodes()[0]))
			{
				return Info;
			}
		}
	}

	return nullptr;
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
