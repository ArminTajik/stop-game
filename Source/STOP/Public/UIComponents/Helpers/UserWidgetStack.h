
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetStack.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All);

/**
 * 
 */
UCLASS()
class STOP_API UUserWidgetStack : public UObject
{
	GENERATED_BODY()

public:
	// UI stack to manage different menus
	UPROPERTY(BlueprintReadOnly)
	TArray<UUserWidget*> UIStack;

	// Function to push a new UI onto the stack
	UFUNCTION(BlueprintCallable, Category="UI")
	void PushUI(UUserWidget* NewUI);

	// Function to pop the current UI from the stack
	UFUNCTION(BlueprintCallable, Category="UI")
	void PopUI();

	// Function to get the current UI
	UFUNCTION(BlueprintCallable, Category="UI")
	UUserWidget* GetCurrentUI() const;

	// Function to clear the entire UI stack
	UFUNCTION(BlueprintCallable, Category="UI")
	void ClearUIStack();
};
