// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_Dialogue.h"
#include "DialogueExtraBehaviour.h"
#include "DialogueComponent_Base.h"
#include "FlowExtraFunctionLibrary.h"

UFlowNode_Dialogue::UFlowNode_Dialogue(const FObjectInitializer& ObjectInitializer)
{
	OutputPins = { DefaultOutputPin };
}

void UFlowNode_Dialogue::ContinueDialogue_Implementation(int SelectionIndex)
{
	// If current text index is max
	if (IsLastText())
	{
		if (HasOptions())
		{
			if (SelectionIndex >= 0)
			{
				TArray<FName> OptionsKeys;
				Options.GenerateKeyArray(OptionsKeys);
				TriggerOutput(OptionsKeys[SelectionIndex], true);
			}
		}
		else
		{
			// Only trigger when index is -1;
			if (SelectionIndex < 0)
			{
				TriggerFirstOutput(true);
			}
		}
	}
	else
	{
		CurrentTextIndex ++;
		if (const auto DialogueFlow = GetDialogueFlowRecursively())
		{
			for (const auto Actor : DialogueFlow->GetIdentityActors())
			{
				if (const auto Comp = Actor->GetComponentByClass<UDialogueComponent_Base>())
				{
					Comp->OnDialogueNodeTextChanged.Broadcast(this);
				}
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

UFlowAsset_Dialogue* UFlowNode_Dialogue::GetDialogueFlowRecursively() const
{
	const auto FlowAsset = GetFlowAsset();
	if (const auto DialogueFlow = Cast<UFlowAsset_Dialogue>(FlowAsset))
	{
		return DialogueFlow;
	}

	auto Parent = FlowAsset->GetParentInstance();
	while (Parent)
	{
		if (const auto DialogueFlow = Cast<UFlowAsset_Dialogue>(Parent))
		{
			return DialogueFlow;
		}

		Parent = Parent->GetParentInstance();
	}

	return nullptr;
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

	const auto Flow_Dialogue = GetDialogueFlowRecursively();
	if (Flow_Dialogue)
	{
		Flow_Dialogue->CurrentDialogueNode = this;
	}
	
	if (DialogueCameraCalculation)
	{
		DialogueCameraCalculation->InstanceDialogueNode = this;
		DialogueCameraCalculation->SetupVariables();
	}

	if (Flow_Dialogue)
	{
		for (const auto Actor : Flow_Dialogue->GetIdentityActors())
		{
			if (const auto Comp = Actor->GetComponentByClass<UDialogueComponent_Base>())
			{
				Comp->OnDialogueNodeStart.Broadcast(this);
				Comp->OnDialogueNodeTextChanged.Broadcast(this);
			}
		}
	}

	for (auto ExtraBehaviour : DialogueExtraBehaviours)
	{
		if (ExtraBehaviour)
		{
			ExtraBehaviour->OnDialogueNodeStart(this);
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

	if (const auto Flow_Dialogue = GetDialogueFlowRecursively())
	{
		for (const auto Actor : Flow_Dialogue->GetIdentityActors())
		{
			if (const auto Comp = Actor->GetComponentByClass<UDialogueComponent_Base>())
			{
				Comp->OnDialogueNodeEnd.Broadcast(this);
			}
		}
	}

	for (auto ExtraBehaviour : DialogueExtraBehaviours)
	{
		if (ExtraBehaviour)
		{
			ExtraBehaviour->OnDialogueNodeEnd(this);
		}
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
		TArray<FFlowPin> Pins;
		if (Options.Num() > 0)
		{
			for (auto Option : Options)
			{
				FFlowPin Pin;
				Pin.PinName = Option.Key;
				Pins.AddUnique(Pin);
			}
		}

		RebuildPinArray(Pins, OutputPins, DefaultOutputPin);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
	
#define LOCTEXT_NAMESPACE "DialogueFlow"
	for (int i = 0; i < Text.Num(); ++i)
	{
		auto& Itr = Text[i];
		Itr = FText::ChangeKey(TEXT("DialogueFlow"),
			FString::Printf(TEXT("%s_Text_%i"), *DialogueId, i), Itr);
	}

	for (auto& Pair : Options)
	{
		Pair.Value = FText::ChangeKey(TEXT("DialogueFlow"),
			FString::Printf(TEXT("%s_Option_%s"), *DialogueId, *Pair.Key.ToString()), Pair.Value);
	}
#undef LOCTEXT_NAMESPACE
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