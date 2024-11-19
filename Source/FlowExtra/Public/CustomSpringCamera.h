// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomSpringArm.h"
#include "DialogueCameraCalculation.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "CustomSpringCamera.generated.h"

UCLASS(Blueprintable, BlueprintType)
class FLOWEXTRA_API ACustomSpringCamera : public AActor
{
	GENERATED_BODY()

public:
	ACustomSpringCamera();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCustomSpringArm* SpringArm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDialogueCameraCalculation* DialogueCameraCalculation;
	
	virtual void Tick(float DeltaTime) override;
};
