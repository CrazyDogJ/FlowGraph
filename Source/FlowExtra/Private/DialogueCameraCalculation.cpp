// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueCameraCalculation.h"

void UDialogueCameraCalculation::CalculateCamera_Implementation(FVector& Location, FRotator& Rotation, float& FOV, FVector& PivotLocation)
{
	Location = FVector::Zero();
	Rotation = FRotator::ZeroRotator;
	FOV = 90.0f;
	PivotLocation = FVector::Zero();
}

TArray<AActor*> UDialogueCameraCalculation::GetIgnoreActors_Implementation()
{
	return TArray<AActor*>();
}
