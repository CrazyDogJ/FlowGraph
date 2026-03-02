// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_ToggleDataLayer.h"

#include "FlowAsset.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

void UFlowNode_ToggleDataLayer::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	auto DataLayerManager = UDataLayerManager::GetDataLayerManager(GetFlowAsset()->GetOwner()->GetWorld());
	if (DataLayerManager->IsValidLowLevel())
	{
		for (const auto Itr : DataLayerAssets)
		{
			if (Itr.DataLayerAsset.IsValid())
			{
				auto LoadedDataLayer = Itr.DataLayerAsset.LoadSynchronous();
				DataLayerManager->SetDataLayerRuntimeState(LoadedDataLayer, Itr.DataLayerState);
			}
		}
	}

	TriggerFirstOutput(true);
}

#if WITH_EDITOR
FString UFlowNode_ToggleDataLayer::GetNodeCategory() const
{
	return "Quest";
}

FString UFlowNode_ToggleDataLayer::GetNodeDescription() const
{
	FString Description;
	for (const auto Itr : DataLayerAssets)
	{
		FString DataLayerName = "NULL";
		if (Itr.DataLayerAsset.IsValid())
		{
			auto LoadedDataLayer = Itr.DataLayerAsset.LoadSynchronous();
			DataLayerName = LoadedDataLayer->GetName();
		}
		Description += DataLayerName + " : " + StaticEnum<EDataLayerRuntimeState>()->GetDisplayNameTextByValue(static_cast<int>(Itr.DataLayerState)).ToString() + LINE_TERMINATOR;
	}
	Description.RemoveFromEnd(LINE_TERMINATOR);
	return Description;
}
#endif