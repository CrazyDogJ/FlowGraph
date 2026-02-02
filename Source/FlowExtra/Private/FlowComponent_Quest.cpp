#include "FlowComponent_Quest.h"

#include "FlowExtraGameplayTags.h"
#include "FlowSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

UFlowComponent_Quest::UFlowComponent_Quest(const FObjectInitializer& ObjectInitializer)
{
	IdentityTags.AddTag(FlowQuestTags::FlowQuestComp);
}

void UFlowComponent_Quest::BeginPlay()
{
	for (auto QuestDelegate : QuestDelegates)
	{
		QuestDelegate->OwnerQuestFlowComp = this;
	}

	// Call Delegates
	auto Comp = UGameplayStatics::GetGameState(GetWorld())->GetComponentByClass<UQuestGlobalComponent>();
	auto QuestInstances = GetFlowSubsystem()->GetRootInstancesByOwner(Comp);
	for (auto Instance : QuestInstances)
	{
		if (auto FlowAsset_Quest = Cast<UFlowAsset_Quest>(Instance->GetTemplateAsset()))
		{
			if (auto FoundDelegate = FindDelegateByFlow(FlowAsset_Quest))
			{
				FoundDelegate->OnQuestStart(FlowAsset_Quest);
				if (auto FoundNode = FindQuestNode(FlowAsset_Quest, FoundDelegate))
				{
					FoundDelegate->OnQuestNodeStateChanged(FoundNode, EGS_Ongoing);
				}
			}
		}
	}
	
	Super::BeginPlay();
}

UQuestDelegate* UFlowComponent_Quest::FindDelegateByFlow(UFlowAsset_Quest* Template)
{
	for (auto Delegate : QuestDelegates)
	{
		if (Delegate->ListeningQuest == Template)
		{
			return Delegate;
		}
	}
	return nullptr;
}

UFlowNode_QuestCommon* UFlowComponent_Quest::FindQuestNode(const UFlowAsset_Quest* Instance, const UQuestDelegate* Delegate)
{
	for (auto Node : Instance->GetActiveNodes())
	{
		if (auto QuestNode = Cast<UFlowNode_QuestCommon>(Node))
		{
			if (Delegate->ListeningQuestGoals.Find(QuestNode->NodeGuid) >= 0)
			{
				return QuestNode;
			}
		}
	}
	return nullptr;
}
