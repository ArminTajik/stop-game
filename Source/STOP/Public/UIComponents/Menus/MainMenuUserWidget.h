
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUserWidget.generated.h"

class UButtonUW;
DECLARE_LOG_CATEGORY_EXTERN(LogMainMenuUserWidget, Log, All);

class UButton;

/**
 * 
 */
UCLASS()
class STOP_API UMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// UI Elements ==========================================================================

	UPROPERTY(meta = (BindWidget))
	UButtonUW* SelectLevelButton;

	UPROPERTY(meta = (BindWidget))
	UButtonUW* OptionsButton;

	UPROPERTY(meta = (BindWidget))
	UButtonUW* CreditsButton;

	UPROPERTY(meta = (BindWidget))
	UButtonUW* StoryButton;

	// Functions ==========================================================================
	virtual void NativeOnInitialized() override;
	void BindWidgets();

	UFUNCTION()
	void OnSelectLevelButtonClicked();

	UFUNCTION()
	void OnControlsButtonClicked();

	UFUNCTION()
	void OnOptionsButtonClicked();

	UFUNCTION()
	void OnCreditsButtonClicked();

	UFUNCTION()
	void OnStoryButtonClicked();
};
