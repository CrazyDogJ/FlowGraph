// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowExtraFunctionLibrary.h"

#include "FlowAsset.h"
#include "FlowSave.h"

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
