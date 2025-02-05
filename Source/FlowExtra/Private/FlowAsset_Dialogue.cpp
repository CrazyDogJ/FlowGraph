// Fill out your copyright notice in the Description page of Project Settings.

#include "FlowAsset_Dialogue.h"

#include "DialogueComponent_Base.h"
#include "FlowComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UFlowAsset_Dialogue::FinishFlow(const EFlowFinishPolicy InFinishPolicy, const bool bRemoveInstance)
{
	CurrentDialogueNode = nullptr;
	
	for (auto Actor : GetIdentityActors())
	{
		if (const auto Comp = Cast<UDialogueComponent_Base>(Actor->GetComponentByClass(UDialogueComponent_Base::StaticClass())))
		{
			Comp->OnDialogueFlowEnd.Broadcast(this);
		}
	}
	
	Super::FinishFlow(InFinishPolicy, bRemoveInstance);
}

TArray<AActor*> UFlowAsset_Dialogue::GetIdentityActors()
{
	TArray<TObjectPtr<AActor>> Result;
	IdentityActors.GenerateValueArray(Result);
	return Result;
}

void UFlowAsset_Dialogue::UpdateNearNPC()
{
	// Setup Ignore Actors
	TArray<AActor*> IgnoreActors;
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	auto PlayerTag = TagsManager.RequestGameplayTag(TEXT("Flow.DialogInvolver.Player"));
	auto DialogOwnerTag = TagsManager.RequestGameplayTag(TEXT("Flow.DialogInvolver.DialogOwner"));
	if (auto Found = IdentityActors.Find(PlayerTag))
	{
		IgnoreActors.Add(Found->Get());
	}
	if (auto Found = IdentityActors.Find(DialogOwnerTag))
	{
		IgnoreActors.Add(Found->Get());
	}

	// Find center actor
	TObjectPtr<AActor> CenterActor;
	if (auto Found = IdentityActors.Find(CenterActorTag))
	{
		CenterActor = Found->Get();
	}
	if (!CenterActor)
	{
		return;
	}

	// Radius sweep
	TArray<TEnumAsByte<EObjectTypeQuery>> QueryObjectTypes;
	QueryObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> RadiusSweepResult;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), CenterActor->GetActorLocation(), Radius, QueryObjectTypes,
											  AActor::StaticClass(), IgnoreActors, RadiusSweepResult);

	// Valid visible trace
	FCollisionQueryParams LineTraceQueryParams;
	LineTraceQueryParams.AddIgnoredActors(IgnoreActors);
	for (auto Actor : RadiusSweepResult)
	{
		bool Trace = GetWorld()->LineTraceTestByChannel(CenterActor->GetActorLocation(), Actor->GetActorLocation(),
														ECC_Visibility, LineTraceQueryParams);
		if (!Trace)
		{
			// Update list
			if (auto FlowComponent = Cast<UFlowComponent>(Actor->GetComponentByClass(UFlowComponent::StaticClass())))
			{
				FGameplayTagContainer Container = FGameplayTagContainer::CreateFromArray(IdentityTags);
				auto MatchContainer = FlowComponent->IdentityTags.FilterExact(Container);
				if (MatchContainer.GetByIndex(0).IsValid())
				{
					IdentityActors.Add(MatchContainer.GetByIndex(0), Actor);
				}
			}
		}
	}
}

void UFlowAsset_Dialogue::SetupVariables(AActor* Player, AActor* DialogueObject)
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	auto PlayerTag = TagsManager.RequestGameplayTag(TEXT("Flow.DialogInvolver.Player"));
	auto DialogOwnerTag = TagsManager.RequestGameplayTag(TEXT("Flow.DialogInvolver.DialogOwner"));
	
	IdentityActors.Add(PlayerTag, Player);
	IdentityActors.Add(DialogOwnerTag, DialogueObject);

	UpdateNearNPC();
	
	for (const auto Actor : GetIdentityActors())
	{
		if (const auto Comp = Cast<UDialogueComponent_Base>(Actor->GetComponentByClass(UDialogueComponent_Base::StaticClass())))
		{
			Comp->OnDialogueFlowStart.Broadcast(this);
		}
	}

	StartFlow();
}
