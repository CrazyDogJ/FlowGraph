#pragma once

#include "CoreMinimal.h"
#include "FlowComponent.h"
#include "QuestDelegate.h"
#include "FlowComponent_Quest.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLOWEXTRA_API UFlowComponent_Quest : public UFlowComponent
{
	GENERATED_BODY()

	UFlowComponent_Quest(const FObjectInitializer& ObjectInitializer);
	
protected:
	
	virtual void BeginPlay() override;

public:

	void CallEvents();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	TArray<UQuestDelegate*> QuestDelegates;

	UQuestDelegate* FindDelegateByFlow(UFlowAsset_Quest* Template);

	UFlowNode_QuestCommon* FindQuestNode(const UFlowAsset_Quest* Instance, const UQuestDelegate* Delegate);
};
