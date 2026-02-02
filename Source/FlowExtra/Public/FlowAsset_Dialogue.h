#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "FlowNode_Dialogue.h"
#include "FlowAsset_Dialogue.generated.h"

UCLASS(BlueprintType, hideCategories = Object)
class FLOWEXTRA_API UFlowAsset_Dialogue : public UFlowAsset
{
	GENERATED_BODY()

	virtual void FinishFlow(const EFlowFinishPolicy InFinishPolicy, const bool bRemoveInstance) override;
	
public:
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
	 * Current dialogue instance identity actors.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TMap<FGameplayTag, TObjectPtr<AActor>> IdentityActors;

	/**
	 * Current active dialogue node.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Flow|Extra")
	UFlowNode_Dialogue* CurrentDialogueNode;

public:
	AActor* FindIdentityActors(FGameplayTag InTag);
	TArray<AActor*> GetIdentityActors() const;
	void UpdateNearNPC();
	
	/** Call it when dialogue flow is created. */
	void SetupVariables(AActor* Player, AActor* DialogueObject);
};
