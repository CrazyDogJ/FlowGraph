// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomSpringCamera.h"

#include "CustomSpringArm.h"

ACustomSpringCamera::ACustomSpringCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<UCustomSpringArm>(TEXT("CustomSpringArm"));
	SetRootComponent(SpringArm);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ACustomSpringCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!DialogueCameraCalculation)
	{
		return;
	}

	SpringArm->IgnoreActors = DialogueCameraCalculation->GetIgnoreActors();
	FVector Location;
	FRotator Rotation;
	float FOV;
	FVector PivotLocation;
	DialogueCameraCalculation->CalculateCamera(DeltaTime, Location, Rotation, FOV, PivotLocation);
	SetActorTransform(FTransform(Rotation, PivotLocation, FVector(1,1,1)));
	Camera->FieldOfView = FOV;
	float BlendAlpha;
	auto Settings = DialogueCameraCalculation->CalculateCameraPostProcess(DeltaTime, BlendAlpha);
	Camera->PostProcessSettings = Settings;
	Camera->PostProcessBlendWeight = BlendAlpha;
	SpringArm->TargetArmLength = FVector::Distance(GetActorLocation(), Location);
}

