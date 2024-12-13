// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestInfo.h"

UFlowNode_QuestInfo::UFlowNode_QuestInfo()
{
#if WITH_EDITOR
	Category = TEXT("Quest");
#endif
}

void UFlowNode_QuestInfo::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	TriggerFirstOutput(false);
}
