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
	Super::BeginPlay();

	CallEvents();
}

void UFlowComponent_Quest::CallEvents()
{
	auto Comp = UGameplayStatics::GetGameState(GetWorld())->GetComponentByClass<UQuestGlobalComponent>();
	
	for (auto QuestDelegate : QuestDelegates)
	{
		QuestDelegate->OwnerQuestFlowComp = this;
		
		const auto FlowState = Comp->GetQuestFlowState(QuestDelegate->ListeningQuest);
		if (FlowState == QFS_Ongoing)
		{
			QuestDelegate->OnQuestStart();
		}
		else if (FlowState == QFS_Finished || FlowState == QFS_Failed)
		{
			QuestDelegate->OnQuestEnd(FlowState);
		}
		
		for (const auto Node : QuestDelegate->ListeningQuestGoals)
		{
			TEnumAsByte<EGoalState> GoalState;
			Comp->GetGoalState(Node, GoalState);
			QuestDelegate->OnQuestNodeStateChanged(Node, GoalState);
		}
	}
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
