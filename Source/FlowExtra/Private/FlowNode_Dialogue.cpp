// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_Dialogue.h"
#include "DialogueExtraBehaviour.h"
#include "DialogueComponent_Base.h"
#include "FlowExtraFunctionLibrary.h"

UFlowNode_Dialogue::UFlowNode_Dialogue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OutputPins = { DefaultOutputPin };
}

void UFlowNode_Dialogue::ContinueDialogue_Implementation(int SelectionIndex)
{
	// If current text index is max
	if (IsLastText())
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
	else
	{
		CurrentTextIndex ++;
		for (auto Actor : Cast<UFlowAsset_Dialogue>(GetFlowAsset())->GetIdentityActors())
		{
			if (const auto Comp = Cast<UDialogueComponent_Base>(Actor->GetComponentByClass(UDialogueComponent_Base::StaticClass())))
			{
				Comp->OnDialogueNodeTextChanged.Broadcast(this);
			}
		}
	}
}

UFlowNode_Dialogue::UFlowNode_Dialogue()
{
#if WITH_EDITOR
	Category = TEXT("Dialogue");
#endif
}

bool UFlowNode_Dialogue::HasOptions() const
{
	return Options.Num() > 0;
}

bool UFlowNode_Dialogue::IsLastText() const
{
	return CurrentTextIndex == Text.Num() - 1;
}

void UFlowNode_Dialogue::ExecuteInput(const FName& PinName)
{
	if (Text.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("This node has no text, flow asset is %s"), *GetFlowAsset()->GetName())
		Finish();
		return;
	}
	
	CurrentTextIndex = 0;
	
	if (auto Flow_Dialogue = Cast<UFlowAsset_Dialogue>(GetFlowAsset()))
	{
		Flow_Dialogue->CurrentDialogueNode = this;
	}
	
	if (DialogueCameraCalculation)
	{
		DialogueCameraCalculation->InstanceDialogueNode = this;
		DialogueCameraCalculation->SetupVariables();
	}
	
	for (auto Actor : Cast<UFlowAsset_Dialogue>(GetFlowAsset())->GetIdentityActors())
	{
		if (const auto Comp = Cast<UDialogueComponent_Base>(Actor->GetComponentByClass(UDialogueComponent_Base::StaticClass())))
		{
			Comp->OnDialogueNodeStart.Broadcast(this);
			Comp->OnDialogueNodeTextChanged.Broadcast(this);
		}
	}

	for (auto ExtraBehaviour : DialogueExtraBehaviours)
	{
		ExtraBehaviour->OnDialogueNodeStart(this);
	}
	
	Super::ExecuteInput(PinName);
}

void UFlowNode_Dialogue::Finish()
{
	if (DialogueCameraCalculation)
	{
		DialogueCameraCalculation->ClearCamera();
	}
	
	for (auto Actor : Cast<UFlowAsset_Dialogue>(GetFlowAsset())->GetIdentityActors())
	{
		if (const auto Comp = Cast<UDialogueComponent_Base>(Actor->GetComponentByClass(UDialogueComponent_Base::StaticClass())))
		{
			Comp->OnDialogueNodeEnd.Broadcast(this);
		}
	}

	for (auto ExtraBehaviour : DialogueExtraBehaviours)
	{
		ExtraBehaviour->OnDialogueNodeEnd(this);
	}

	CurrentTextIndex = -1;
	
	Super::Finish();
}

#if WITH_EDITOR
void UFlowNode_Dialogue::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property
		&& PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UFlowNode_Dialogue, Options))
	{
		if (Options.Num() > 0)
		{
			OutputPins.Empty();
			for (auto Option : Options)
			{
				FFlowPin Pin;
				Pin.PinName = Option.Key;
				OutputPins.AddUnique(Pin);
			}
		}
		else
		{
			OutputPins = { DefaultOutputPin };
		}
		OnReconstructionRequested.ExecuteIfBound();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

FString UFlowNode_Dialogue::GetNodeDescription() const
{
	FString ArrayText;
	for (auto Single : Text)
	{
		ArrayText = ArrayText + Single.ToString() + LINE_TERMINATOR;
	}
	return ArrayText + K2_GetNodeDescription();
}
#endif