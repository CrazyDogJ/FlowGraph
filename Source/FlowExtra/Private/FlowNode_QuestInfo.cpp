// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowNode_QuestInfo.h"

void UFlowNode_QuestInfo::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	TriggerFirstOutput(false);
}
