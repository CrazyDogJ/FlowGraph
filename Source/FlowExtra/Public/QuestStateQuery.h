// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestGlobalComponent.h"
#include "UObject/Object.h"
#include "QuestStateQuery.generated.h"

class UFlowAsset_Quest;

UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
class UQuestQueryChild : public UObject
{
	GENERATED_BODY()

public:
	UQuestQueryChild() {}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bNegative = false;
	
	virtual bool GetResult(const UQuestGlobalComponent* Component) const { return false; }
};

UCLASS()
class UQuestQuery_QuestState : public UQuestQueryChild
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UFlowAsset_Quest* QuestFlowTemplate = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EQuestFlowState> ExpectedState = QFS_Ongoing;

	virtual bool GetResult(const UQuestGlobalComponent* Component) const override
	{
		if (Component)
		{
			bool bResult = Component->GetQuestFlowState(QuestFlowTemplate) == ExpectedState;
			if (bNegative)
			{
				return bResult = !bResult;
			}
			
			return bResult;
		}
		
		return false;
	}
};

UCLASS()
class UQuestQuery_GoalState : public UQuestQueryChild
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGuid QuestNodeGuid = FGuid();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EGoalState> ExpectedState = EGS_Ongoing;
	
	virtual bool GetResult(const UQuestGlobalComponent* Component) const override
	{
		if (Component)
		{
			TEnumAsByte<EGoalState> Result;
			Component->GetGoalState(QuestNodeGuid, Result);
			bool FinalBool = Result == ExpectedState;
			
			if (bNegative)
			{
				FinalBool = !FinalBool;
			}
			
			return FinalBool;
		}
		
		return false;
	}
};

UCLASS()
class UQuestQuery_And : public UQuestQueryChild
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UQuestQueryChild* QueryDataLeft;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UQuestQueryChild* QueryDataRight;

	virtual bool GetResult(const UQuestGlobalComponent* Component) const override
	{
		if (QueryDataLeft && QueryDataRight)
		{
			bool Result = QueryDataLeft->GetResult(Component) && QueryDataRight->GetResult(Component);
			if (bNegative)
			{
				Result = !Result;
			}
			return Result;
		}

		return false;
	}
};

UCLASS()
class UQuestQuery_Or : public UQuestQueryChild
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UQuestQueryChild* QueryDataLeft;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UQuestQueryChild* QueryDataRight;

	virtual bool GetResult(const UQuestGlobalComponent* Component) const override
	{
		if (QueryDataLeft && QueryDataRight)
		{
			bool Result = QueryDataLeft->GetResult(Component) || QueryDataRight->GetResult(Component);
			if (bNegative)
			{
				Result = !Result;
			}
			return Result;
		}

		return false;
	}
};

USTRUCT(BlueprintType)
struct FQuestStateQuery
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UQuestQueryChild* QueryData;

	bool GetResult(const UQuestGlobalComponent* Component) const
	{
		if (QueryData)
		{
			return QueryData->GetResult(Component);
		}

		return false;
	}
};
