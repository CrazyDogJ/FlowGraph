// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "CustomSpringArm.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLOWEXTRA_API UCustomSpringArm : public USpringArmComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCustomSpringArm();

	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> IgnoreActors;
};
