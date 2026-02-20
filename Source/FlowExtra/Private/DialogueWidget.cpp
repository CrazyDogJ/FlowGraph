// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

#include "FlowExtraFunctionLibrary.h"

UDialogueWidget::UDialogueWidget(const FObjectInitializer& Initializer)
	: Super(Initializer), ButtonPool(FUserWidgetPool(*this))
{
}

UUserWidget* UDialogueWidget::GetOrCreateSelectionButton()
{
	return ButtonPool.GetOrCreateInstance(SelectionButtonClass);
}

void UDialogueWidget::HideList()
{
	if (const auto List = GetSelectionButtonList())
		List->SetVisibility(ESlateVisibility::Collapsed);
	
	for (const auto Itr : ButtonPool.GetActiveWidgets())
	{
		Itr->RemoveFromParent();
	}

	OptionsWidgets.Empty();
	ButtonPool.ReleaseAll();
}

void UDialogueWidget::ShowList(const TMap<FName, FText>& InOptions)
{
	TArray<UUserWidget*> Widgets;
	for (int i = 0; i < InOptions.Num(); ++i)
	{
		if (const auto NewButtonWidget = GetOrCreateSelectionButton())
		{
			Widgets.Add(NewButtonWidget);
		}
	}
	
	OptionsWidgets = Widgets;
	AddButtons();
	
	if (const auto List = GetSelectionButtonList())
		List->SetVisibility(ESlateVisibility::Visible);
}

void UDialogueWidget::StartTextPopTask(const FText InText)
{
	CurrentTextPop = InText;
	CurrentStringPop = FString();
	PopTime = 0.0f;
	PopCharacterIndex = 0;
	SetTextPoping(true);
}

void UDialogueWidget::SkipTextPop()
{
	if (bTextPoping)
	{
		SetTextPoping(false);
		PopTime = 0.0f;
		PopCharacterIndex = 0;
		CurrentStringPop = CurrentTextPop.ToString();
	}
}

void UDialogueWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	const auto CharArray = CurrentTextPop.ToString().GetCharArray();
	if (bTextPoping)
	{
		PopTime += InDeltaTime;
		if (PopTime > TextPopPeriod)
		{
			if (CharArray.IsValidIndex(PopCharacterIndex))
			{
				CurrentStringPop.AppendChar(CharArray[PopCharacterIndex]);
			}
			PopCharacterIndex ++;
			// Character pop end!
			if (PopCharacterIndex >= CharArray.Num())
			{
				SetTextPoping(false);
			}
			PopTime = 0.0f;
		}
	}
}

void UDialogueWidget::SetTextPoping(const bool In)
{
	bTextPoping = In;
	
	if (bTextPoping)
	{
		HideList();
		return;
	}
	
	if (const auto Node = UFlowExtraFunctionLibrary::GetCurrentDialogueNode(DialogueComponent))
	{
		if (Node->HasOptions())
		{
			ShowList(Node->Options);
		}
		else
		{
			HideList();
		}
	}
}
