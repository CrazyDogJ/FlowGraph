// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueComponent_Base.h"

#include "CustomSpringCamera.h"
#include "DialogueCameraInterface.h"
#include "DialogueWidget.h"
#include "FlowAsset.h"
#include "FlowAsset_Dialogue.h"
#include "FlowExtraGameplayTags.h"
#include "FlowSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UDialogueComponent_Base::UDialogueComponent_Base()
{
	SetIsReplicatedByDefault(true);
}

void UDialogueComponent_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentDialogueMontage);
	DOREPLIFETIME(ThisClass, bInDialogue);
}

void UDialogueComponent_Base::OnDialogueNodeStartEvent(UFlowNode_Dialogue* DialogueNode)
{
	if (auto Pawn = Cast<APawn>(GetOwner()))
	{
		if (Pawn->IsLocallyControlled() && DialogueNode->DialogueCameraCalculation && !Pawn->IsBotControlled())
		{
			if (CurrentCamera)
			{
				CurrentCamera->Destroy();
				CurrentCamera = nullptr;
			}
			
			if (DialogueNode->DialogueCameraCalculation->bSpawnNewCamera)
			{
				CurrentCamera = GetWorld()->SpawnActor<ACustomSpringCamera>(ACustomSpringCamera::StaticClass());
				CurrentCamera->DialogueCameraCalculation = DialogueNode->DialogueCameraCalculation;
				IDialogueCameraInterface::Execute_SetNewViewTarget(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0), CurrentCamera);
			}
			else
			{
				IDialogueCameraInterface::Execute_SetNewViewTarget(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0), DialogueNode->DialogueCameraCalculation->GetViewActor());
			}
		}
	}

	for (auto Pair : DialogueNode->DialogueAnimation)
	{
		if (FindRole(Pair.Key))
		{
			if (Pair.Value.bStopPreviousMontageOnStart)
			{
				CharacterPlayMontage(nullptr, EDMM_StopCurrent);
			}
			CharacterPlayMontage(Pair.Value.Montage, EDMM_Play);
			TArray<FName> MorphNames;
			TArray<float> MorphWeights;
			Pair.Value.Morphs.GenerateKeyArray(MorphNames);
			Pair.Value.Morphs.GenerateValueArray(MorphWeights);
			CharacterSetMorphs(MorphNames, MorphWeights, true);
		}
	}
}

void UDialogueComponent_Base::OnDialogueNodeEndEvent(UFlowNode_Dialogue* DialogueNode)
{
	for (auto Pair : DialogueNode->DialogueAnimation)
	{
		if (FindRole(Pair.Key))
		{
			if (Pair.Value.bStopMontageOnEnd)
			{
				CharacterPlayMontage(Pair.Value.Montage, EDMM_StopInput);
			}
			TArray<FName> MorphNames;
			TArray<float> MorphWeights;
			Pair.Value.Morphs.GenerateKeyArray(MorphNames);
			Pair.Value.Morphs.GenerateValueArray(MorphWeights);
			CharacterSetMorphs(MorphNames, MorphWeights, false);
			break;
		}
	}
}

void UDialogueComponent_Base::OnDialogueFlowStartEvent(UFlowAsset_Dialogue* DialogueFlow)
{
	CurrentDialogueInstance = DialogueFlow;
	
	const auto DialoguePlayer = CurrentDialogueInstance->IdentityActors.Find(FlowDialogueTags::FlowDialoguePlayer);
	const auto DialogueFlowOwner = CurrentDialogueInstance->IdentityActors.Find(FlowDialogueTags::FlowDialogueOwner);

	if (!DialoguePlayer)
	{
		return;
	}

	// Check if interacted player is owner.
	const auto Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->IsLocalPlayerControllerViewingAPawn())
	{
		DialogueWidget = CreateWidget<UDialogueWidget>(Pawn->GetLocalViewingPlayerController(), WidgetClass);
		if (DialogueWidget)
		{
			if (DialogueFlowOwner)
			{
				const auto DialogueComp = DialogueFlowOwner->Get()->GetComponentByClass<UDialogueComponent_Base>();
					
				DialogueWidget->DialogueComponent = DialogueComp;
				DialogueWidget->AddToViewport();
				Pawn->GetLocalViewingPlayerController()->SetShowMouseCursor(true);
				UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(Pawn->GetLocalViewingPlayerController(), DialogueWidget, EMouseLockMode::DoNotLock, true);
			}
		}
	}

	bInDialogue = true;
}

void UDialogueComponent_Base::OnDialogueFlowEndEvent(UFlowAsset_Dialogue* DialogueFlow)
{
	if (CurrentDialogueMontage)
	{
		CharacterPlayMontage(nullptr, EDMM_StopCurrent);
	}

	if (auto Pawn = Cast<APawn>(GetOwner()))
	{
		if (Pawn->IsLocallyControlled())
		{
			if (DialogueWidget)
			{
				DialogueWidget->GetOwningPlayer()->SetShowMouseCursor(false);
				UWidgetBlueprintLibrary::SetInputMode_GameOnly(DialogueWidget->GetOwningPlayer(), true);
				DialogueWidget->RemoveFromParent();
				DialogueWidget = nullptr;
			}
			if (CurrentCamera)
			{
				CurrentCamera->Destroy();
				CurrentCamera = nullptr;
				if (IDialogueCameraInterface* CameraMangerInterface = Cast<IDialogueCameraInterface>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
				{
					CameraMangerInterface->Execute_SetNewViewTarget(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0), nullptr);
				}
			}
		}
	}

	CurrentDialogueInstance = nullptr;
	bInDialogue = false;
}

void UDialogueComponent_Base::CharacterPlayMontage_Implementation(UAnimMontage* AnimMontage, EDialogMontageMode Mode)
{
	// Invalidate
	if (!ActorPrimitiveComponent)
	{
		return;
	}
	auto Skel = Cast<USkeletalMeshComponent>(ActorPrimitiveComponent);
	if (!Skel)
	{
		return;
	}

	switch (Mode)
	{
	case EDMM_Play:
		if (AnimMontage)
		{
			Skel->GetAnimInstance()->Montage_Play(AnimMontage);
			CurrentDialogueMontage = AnimMontage;
		}
		break;
	case EDMM_StopInput:
		if (AnimMontage)
		{
			Skel->GetAnimInstance()->Montage_Stop(AnimMontage->BlendOut.GetBlendTime(), AnimMontage);
			CurrentDialogueMontage = nullptr;
		}
		break;
	case EDMM_StopCurrent:
		if (CurrentDialogueMontage)
		{
			Skel->GetAnimInstance()->Montage_Stop(CurrentDialogueMontage->BlendOut.GetBlendTime(), CurrentDialogueMontage);
			CurrentDialogueMontage = nullptr;
		}
		break;
	}
}

void UDialogueComponent_Base::CharacterSetMorphs_Implementation(const TArray<FName>& MorphNames, const TArray<float>& MorphAlpha,
	bool bSetOrClear)
{
	if (!ActorPrimitiveComponent)
	{
		return;
	}

	if (auto Skel = Cast<USkeletalMeshComponent>(ActorPrimitiveComponent))
	{
		for (int id = 0; id < MorphNames.Num(); id++)
		{
			Skel->SetMorphTarget(MorphNames[id], bSetOrClear ? MorphAlpha[id] : 0.0f);
		}
	}
}

void UDialogueComponent_Base::SetupVariables(UPrimitiveComponent* InPrimitiveComponent, FName InComponentSocket)
{
	ActorPrimitiveComponent = InPrimitiveComponent;
	ComponentSocket = InComponentSocket;
}

void UDialogueComponent_Base::StartDialogue(UFlowAsset_Dialogue* FlowAsset, AActor* InteractedCharacter)
{
	if (!FlowAsset || !InteractedCharacter)
	{
		return;
	}

	if (auto FlowInstance = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>()->CreateRootFlow(this, FlowAsset))
	{
		Cast<UFlowAsset_Dialogue>(FlowInstance)->SetupVariables(InteractedCharacter, GetOwner());
	}
}

bool UDialogueComponent_Base::FindRole(const FGameplayTag& InTag) const
{
	if (const auto Found = CurrentDialogueInstance->IdentityActors.Find(InTag))
	{
		const AActor* Actor = *Found;
		return Actor == GetOwner();
	}
	
	return false;
}

void UDialogueComponent_Base::BeginPlay()
{
	Super::BeginPlay();
	
	OnDialogueNodeStart.AddDynamic(this, &UDialogueComponent_Base::OnDialogueNodeStartEvent);
	OnDialogueNodeEnd.AddDynamic(this, &UDialogueComponent_Base::OnDialogueNodeEndEvent);
	OnDialogueFlowStart.AddDynamic(this, &UDialogueComponent_Base::OnDialogueFlowStartEvent);
	OnDialogueFlowEnd.AddDynamic(this, &UDialogueComponent_Base::OnDialogueFlowEndEvent);
}

void UDialogueComponent_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnDialogueNodeStart.RemoveAll(this);
	OnDialogueNodeEnd.RemoveAll(this);
	OnDialogueFlowStart.RemoveAll(this);
	OnDialogueFlowEnd.RemoveAll(this);
}
