#include "FlowAsset_Dialogue.h"

#include "DialogueComponent_Base.h"
#include "FlowComponent.h"
#include "FlowExtraGameplayTags.h"
#include "FlowSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"

void UFlowAsset_Dialogue::FinishFlow(const EFlowFinishPolicy InFinishPolicy, const bool bRemoveInstance)
{
	CurrentDialogueNode = nullptr;

	// Broadcast flow end events.
	for (auto Actor : GetIdentityActors())
	{
		if (const auto Comp = Actor->GetComponentByClass<UDialogueComponent_Base>())
		{
			Comp->OnDialogueFlowEnd.Broadcast(this);
		}
	}

	IdentityActors.Empty();
	for (const auto CameraActor : IdentityCameraActors)
	{
		if (CameraActor.Value.bIsSpawn)
		{
			CameraActor.Value.CameraActor->Destroy();
		}
	}
	IdentityCameraActors.Empty();
	CurrentDialogueNode = nullptr;
	
	Super::FinishFlow(InFinishPolicy, bRemoveInstance);
}

AActor* UFlowAsset_Dialogue::GetCameraActor(const FGameplayTag& Tag) const
{
	const auto Found = IdentityCameraActors.Find(Tag);
	if (Found != nullptr)
	{
		return Found->CameraActor;
	}

	return nullptr;
}

AActor* UFlowAsset_Dialogue::FindIdentityActors(const FGameplayTag InTag)
{
	if (const auto Found = IdentityActors.Find(InTag))
	{
		return *Found;
	}

	return nullptr;
}

TArray<AActor*> UFlowAsset_Dialogue::GetIdentityActors() const
{
	TArray<TObjectPtr<AActor>> Result;
	IdentityActors.GenerateValueArray(Result);
	return Result;
}

void UFlowAsset_Dialogue::UpdateNearActor()
{
	// Setup Ignore Actors
	const TArray<AActor*> IgnoreActors = GetIdentityActors();

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

	// TODO : Default get nearby logic, will make it virtual later.
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
			if (auto FlowComponent = Actor->GetComponentByClass<UFlowComponent>())
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

void UFlowAsset_Dialogue::UpdatePersistentActor()
{
	for (const auto Itr : PersistentIdentityTags)
	{
		const auto Actors = GetFlowSubsystem()->GetFlowActorsByTag(Itr, AActor::StaticClass());
		const auto ActorsArray = Actors.Array();
		if (ActorsArray.IsValidIndex(0))
		{
			IdentityActors.Add(Itr,ActorsArray[0]);
		}
	}
}

void UFlowAsset_Dialogue::UpdateCameraActors()
{
	for (const auto Itr : IdentityActors)
	{
		if (const auto DialogueComp = Itr.Value->GetComponentByClass<UDialogueComponent_Base>())
		{
			if (const auto NewActor = DialogueComp->RequestCameraActor())
			{
				IdentityCameraActors.Add(Itr.Key, FCameraActorInfo(NewActor, true));
			}
		}
		else
		{
			IdentityCameraActors.Add(Itr.Key, FCameraActorInfo(Itr.Value, false));
		}
	}
}

void UFlowAsset_Dialogue::SetupVariables(AActor* Player, AActor* DialogueObject)
{
	IdentityActors.Empty();
	
	const FGameplayTag PlayerTag = FlowDialogueTags::FlowDialoguePlayer;
	const FGameplayTag DialogOwnerTag = FlowDialogueTags::FlowDialogueOwner;
	
	IdentityActors.Add(PlayerTag, Player);
	IdentityActors.Add(DialogOwnerTag, DialogueObject);

	UpdateNearActor();
	UpdatePersistentActor();

	// Broadcast flow begin event.
	for (const auto Actor : GetIdentityActors())
	{
		if (const auto Comp = Actor->GetComponentByClass<UDialogueComponent_Base>())
		{
			Comp->OnDialogueFlowStart.Broadcast(this);
		}
	}

	UpdateCameraActors();
	
	StartFlow();
}
