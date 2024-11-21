// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueCameraCalculation.h"

void UDialogueCameraCalculation::CalculateCamera_Implementation(float DeltaTime, FVector& Location, FRotator& Rotation, float& FOV, FVector& PivotLocation)
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

AActor* UDialogueCameraCalculation::GetViewActor_Implementation()
{
	return nullptr;
}

FPostProcessSettings UDialogueCameraCalculation::CalculateCameraPostProcess_Implementation(float DeltaTime, float& BlendAlpha)
{
	return FPostProcessSettings();
}
