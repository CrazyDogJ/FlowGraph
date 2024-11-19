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
