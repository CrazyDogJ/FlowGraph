// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_Dialogue.h"

UFlowNode_Dialogue::UFlowNode_Dialogue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OutputPins = { DefaultOutputPin };
}

void UFlowNode_Dialogue::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();

	StartObserving();
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
	return GetNotifyTagsDescription(NotifyTags) + LINE_TERMINATOR + Text.ToString() + LINE_TERMINATOR + K2_GetNodeDescription();
}
#endif