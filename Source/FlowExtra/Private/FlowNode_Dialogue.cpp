// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_Dialogue.h"

#include "DialogueComponent_Base.h"
#include "FlowExtraFunctionLibrary.h"

UFlowNode_Dialogue::UFlowNode_Dialogue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OutputPins = { DefaultOutputPin };
}

void UFlowNode_Dialogue::ContinueDialogue_Implementation(int SelectionIndex)
{
	if (HasOptions() && SelectionIndex >= 0)
	{
		TArray<FName> OptionsKeys;
		Options.GenerateKeyArray(OptionsKeys);
		TriggerOutput(OptionsKeys[SelectionIndex], true);
	}
	else
	{
		TriggerFirstOutput(true);
	}
}

bool UFlowNode_Dialogue::HasOptions() const
{
	return Options.Num() > 0;
}

void UFlowNode_Dialogue::ExecuteInput(const FName& PinName)
{
	if (DialogueCameraCalculation)
	{
		DialogueCameraCalculation->InstanceDialogueNode = this;
		DialogueCameraCalculation->SetupVariables();
	}

	auto StartNode = UFlowExtraFunctionLibrary::GetCurrentDialogueInfos(GetFlowAsset());
	for (auto Actor : StartNode->GetIdentityActors())
	{
		if (const auto Comp = Cast<UDialogueComponent_Base>(Actor->GetComponentByClass(UDialogueComponent_Base::StaticClass())))
		{
			Comp->OnDialogueNodeStart.Broadcast(this);
		}
	}
	
	Super::ExecuteInput(PinName);
}

void UFlowNode_Dialogue::Finish()
{
	if (DialogueCameraCalculation)
	{
		DialogueCameraCalculation->ClearCamera();
	}

	auto StartNode = UFlowExtraFunctionLibrary::GetCurrentDialogueInfos(GetFlowAsset());
	for (auto Actor : StartNode->GetIdentityActors())
	{
		if (const auto Comp = Cast<UDialogueComponent_Base>(Actor->GetComponentByClass(UDialogueComponent_Base::StaticClass())))
		{
			Comp->OnDialogueNodeEnd.Broadcast(this);
		}
	}
	
	Super::Finish();
}

#if WITH_EDITOR
void UFlowNode_Dialogue::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property
		&& (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UFlowNode_Dialogue, Options) || PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UFlowNode_Dialogue, Options)))
	{
		if (Options.Num() > 0)
		{
			OutputPins.Empty();
			for (auto Option = Options.CreateConstIterator(); Option; ++Option)
			{
				FFlowPin Pin;
				Pin.PinName = Option.Key();
				OutputPins.Add(Pin);
			}
		}
		else
		{
			OutputPins = { DefaultOutputPin };
		}
		GetGraphNode()->ReconstructNode();
	}
}

FString UFlowNode_Dialogue::GetNodeDescription() const
{
	return Text.ToString() + LINE_TERMINATOR + K2_GetNodeDescription();
}
#endif