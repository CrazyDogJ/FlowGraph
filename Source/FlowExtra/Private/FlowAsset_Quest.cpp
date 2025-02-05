// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowAsset_Quest.h"

#include "QuestGlobalComponent.h"

UFlowAsset_Quest::UFlowAsset_Quest(FObjectInitializer const&)
{
}

FFlowAssetSaveData UFlowAsset_Quest::SaveQuestInstance()
{
	FFlowAssetSaveData AssetRecord;
	AssetRecord.WorldName = IsBoundToWorld() ? GetWorld()->GetName() : FString();
	AssetRecord.InstanceName = GetName();

	// opportunity to collect data before serializing asset
	OnSave();

	// iterate nodes
	TArray<UFlowNode*> NodesInExecutionOrder;
	GetNodesInExecutionOrder<UFlowNode>(GetDefaultEntryNode(), NodesInExecutionOrder);
	for (UFlowNode* Node : NodesInExecutionOrder)
	{
		if (Node && (Node->GetActivationState() == EFlowNodeState::Active ||
				Node->GetActivationState() == EFlowNodeState::Completed))
		{
			FFlowNodeSaveData NodeRecord;
			Node->SaveInstance(NodeRecord);

			AssetRecord.NodeRecords.Emplace(NodeRecord);
		}
	}

	// serialize asset
	FMemoryWriter MemoryWriter(AssetRecord.AssetData, true);
	FFlowArchive Ar(MemoryWriter);
	Serialize(Ar);

	return AssetRecord;
}

void UFlowAsset_Quest::LoadQuestInstance(const FFlowAssetSaveData& AssetRecord)
{
	FMemoryReader MemoryReader(AssetRecord.AssetData, true);
	FFlowArchive Ar(MemoryReader);
	Serialize(Ar);

	PreStartFlow();

	// iterate graph "from the end", backward to execution order
	// prevents issue when the preceding node would instantly fire output to a not-yet-loaded node
	for (int32 i = 0; i < AssetRecord.NodeRecords.Num(); i++)
	{
		if (UFlowNode* Node = GetNodes().FindRef(AssetRecord.NodeRecords[i].NodeGuid))
		{
			Node->LoadInstance(AssetRecord.NodeRecords[i]);
		}
	}

	OnLoad();
}

#if WITH_EDITOR
void UFlowAsset_Quest::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	// Auto set item name and description.
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UFlowAsset_Quest, QuestID))
	{
		FTextKey Name = FString::Printf(TEXT("QFN_%s"), *QuestID);
		FTextKey Desc = FString::Printf(TEXT("QFD_%s"), *QuestID);
#define LOCTEXT_NAMESPACE "QuestFlow"
		QuestName = QuestName.ChangeKey(TEXT("QuestFlow"), Name, QuestName);
		QuestDescription = QuestDescription.ChangeKey(TEXT("QuestFlow"), Desc, QuestDescription);
#undef LOCTEXT_NAMESPACE
	}
}
#endif