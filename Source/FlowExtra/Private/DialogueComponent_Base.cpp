// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueComponent_Base.h"

#include "CustomSpringCamera.h"
#include "DialogueCameraInterface.h"
#include "DialogueWidget.h"
#include "FlowAsset.h"
#include "FlowAsset_Dialogue.h"
#include "FlowExtraFunctionLibrary.h"
#include "FlowSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UDialogueComponent_Base::UDialogueComponent_Base()
{
	OnDialogueNodeStart.AddDynamic(this, &UDialogueComponent_Base::OnDialogueNodeStartEvent);
	OnDialogueNodeEnd.AddDynamic(this, &UDialogueComponent_Base::OnDialogueNodeEndEvent);
	OnDialogueFlowStart.AddDynamic(this, &UDialogueComponent_Base::OnDialogueFlowStartEvent);
	OnDialogueFlowEnd.AddDynamic(this, &UDialogueComponent_Base::OnDialogueFlowEndEvent);
}

void UDialogueComponent_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentDialogueMontage);
	DOREPLIFETIME(ThisClass, bInDialogue);
	DOREPLIFETIME(ThisClass, CurrentRole);
}

void UDialogueComponent_Base::OnDialogueNodeStartEvent(UFlowNode_Dialogue* DialogueNode)
{
	if (auto Character = Cast<ACharacter>(GetOwner()))
	{
		if (Character->IsLocallyControlled() && DialogueNode->DialogueCameraCalculation && !Character->IsBotControlled())
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
	if (auto Character = Cast<ACharacter>(GetOwner()))
	{
		if (Character->IsLocallyControlled())
		{
			DialogueWidget = CreateWidget<UDialogueWidget>(Character->GetLocalViewingPlayerController(), WidgetClass);
			if (DialogueWidget)
			{
				DialogueWidget->DialogueComponent = this;
				DialogueWidget->AddToViewport();
				Character->GetLocalViewingPlayerController()->SetShowMouseCursor(true);
				UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(Character->GetLocalViewingPlayerController(), DialogueWidget);
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

	if (auto Character = Cast<ACharacter>(GetOwner()))
	{
		if (Character->IsLocallyControlled())
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
	// invalid ptr
	if (!FlowAsset || !InteractedCharacter)
	{
		return;
	}

	// Check is a dialogue flow asset
	//const bool bIsDialogueFlow = Cast<UFlowAsset_Dialogue>(FlowAsset);
	//if (!bIsDialogueFlow)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("You are trying to start a invalid dialogue flow. Please check whether %s has a Dialogue_Start node."), *FlowAsset->GetName());
	//	return;
	//}

	//Start
	if (auto FlowInstance = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>()->CreateRootFlow(this, FlowAsset))
	{
		Cast<UFlowAsset_Dialogue>(FlowInstance)->SetupVariables(InteractedCharacter, GetOwner());
	}
}

bool UDialogueComponent_Base::FindRole(FGameplayTag InTag) const
{
	const bool bIsPlayer = CurrentRole == EDR_Player && InTag ==  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Flow.DialogInvolver.Player"));
	const bool bIsDialogueOwner = CurrentRole == EDR_DialogueOwner && InTag ==  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Flow.DialogInvolver.DialogOwner"));
	bool bIsExtraInvolver = false;
	if (auto FlowComp = Cast<UFlowComponent>(GetOwner()->GetComponentByClass(UFlowComponent::StaticClass())))
	{
		bIsExtraInvolver = CurrentRole == EDR_Extra && FlowComp->IdentityTags.HasTag(InTag);
	}
	
	return bIsPlayer || bIsDialogueOwner || bIsExtraInvolver;
}
