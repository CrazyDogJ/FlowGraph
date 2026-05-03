// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueComponent_Base.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/UserWidgetPool.h"
#include "DialogueWidget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FLOWEXTRA_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UDialogueWidget(const FObjectInitializer& Initializer);
	
	/** Dialogue asset owner component. */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UDialogueComponent_Base* DialogueComponent = nullptr;

	// Time for each character to pop out.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TextPopPeriod = 0.25f;
	
	// Final text should show.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FText CurrentTextPop = FText();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UUserWidget> SelectionButtonClass;

	UPROPERTY(BlueprintReadOnly)
	TArray<UUserWidget*> OptionsWidgets;
	
	UFUNCTION(BlueprintCallable)
	void StartTextPopTask(FText InText);

	UFUNCTION(BlueprintCallable)
	void SkipTextPop();

	UFUNCTION(BlueprintPure)
	bool IsTextPoping() const { return bTextPoping; }
	
	UFUNCTION(BlueprintPure)
	FString GetCurrentTextPopString() const { return CurrentStringPop; }

	UFUNCTION(BlueprintImplementableEvent)
	UWidget* GetSelectionButtonList() const;

	UFUNCTION(BlueprintImplementableEvent)
	void AddButtons();

protected:
	
	UPROPERTY(Transient)
	FUserWidgetPool ButtonPool;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UUserWidget* GetOrCreateSelectionButton();
	void HideList();
	void ShowList(const TMap<FName, FText>& InOptions);
	void SetTextPoping(const bool In);
	
	mutable bool bTextPoping = false;
	
	mutable float PopTime = 0.0f;

	mutable int32 PopCharacterIndex = 0;

	mutable FString CurrentStringPop;
};
