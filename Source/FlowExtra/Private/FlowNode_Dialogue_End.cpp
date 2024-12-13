// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_Dialogue_End.h"

#include "DialogueComponent_Base.h"
#include "FlowAsset.h"
#include "FlowExtraFunctionLibrary.h"

UFlowNode_Dialogue_End::UFlowNode_Dialogue_End()
{
#if WITH_EDITOR
	Category = TEXT("Dialogue");
#endif
}

void UFlowNode_Dialogue_End::ExecuteInput(const FName& PinName)
{
	auto StartNode = UFlowExtraFunctionLibrary::GetCurrentDialogueInfos(GetFlowAsset());
	for (auto Actor : StartNode->GetIdentityActors())
	{
		if (const auto Comp = Cast<UDialogueComponent_Base>(Actor->GetComponentByClass(UDialogueComponent_Base::StaticClass())))
		{
			Comp->OnDialogueFlowEnd.Broadcast(GetFlowAsset());
		}
	}
	
	Super::ExecuteInput(PinName);

	TriggerFirstOutput(true);
}
