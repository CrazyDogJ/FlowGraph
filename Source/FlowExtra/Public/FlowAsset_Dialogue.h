#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "FlowNode_Dialogue.h"
#include "FlowAsset_Dialogue.generated.h"

USTRUCT(BlueprintType)
struct FCameraActorInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* CameraActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsSpawn = false;
};

UCLASS(BlueprintType, hideCategories = Object)
class FLOWEXTRA_API UFlowAsset_Dialogue : public UFlowAsset
{
	GENERATED_BODY()

public:
	virtual void FinishFlow(const EFlowFinishPolicy InFinishPolicy, const bool bRemoveInstance) override;
	
	/**
	 * Tags to identify near dialogue objects;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue|Nearby Detect")
	TArray<FGameplayTag> IdentityTags;

	/**
	 * Radius to check near dialogue objects;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue|Nearby Detect")
	float Radius = 500.0f;

	/**
	 * Center actor;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue|Nearby Detect")
	FGameplayTag CenterActorTag = FGameplayTag::RequestGameplayTag(TEXT("Flow.DialogInvolver.Player"));

	/**
	 * Gameplay tags get from flow subsystem.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue|Register Tags")
	TArray<FGameplayTag> PersistentIdentityTags;
	
	/**
	 * Current dialogue instance identity actors.
	 */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Dialogue")
	TMap<FGameplayTag, TObjectPtr<AActor>> IdentityActors;

	/**
	 * Current dialogue camera actors.
	 */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Dialogue")
	TMap<FGameplayTag, FCameraActorInfo> IdentityCameraActors;
	
	/**
	 * Current active dialogue node.
	 */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Flow|Extra")
	UFlowNode_Dialogue* CurrentDialogueNode;

public:
	UFUNCTION(BlueprintPure)
	AActor* GetCameraActor(const FGameplayTag& Tag) const;
	
	AActor* FindIdentityActors(FGameplayTag InTag);
	TArray<AActor*> GetIdentityActors() const;
	void UpdateNearActor();
	void UpdatePersistentActor();
	void UpdateCameraActors();
	
	/** Call it when dialogue flow is created. */
	void SetupVariables(AActor* Player, AActor* DialogueObject);
};
