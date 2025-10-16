// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"


class ULevelDataAsset;
class UFadeUW;
class UStoryUW;
class UUserWidgetStack;
class USettingsUW;
class UControlsUW;
class UCreditsUW;
class UMainMenuUserWidget;
class USelectLevelUserWidget;
DECLARE_LOG_CATEGORY_EXTERN(LogMainMenuPlayerController, Log, All);

UCLASS()
class AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainMenuPlayerController();

public:
	UFUNCTION()
	void ShowLevelSelectionMenu();

	UFUNCTION()
	void ShowStoryMenu();

	UFUNCTION()
	void ShowSettingsMenu();

	UFUNCTION()
	void ShowCreditsMenu();

	UFUNCTION()
	void ShowControlsMenu();

	UFUNCTION()
	void BackToMainMenu();

	UFUNCTION()
	void HandleStart(ULevelDataAsset* LevelToLoad);

	UFUNCTION()
	void PushWidgetToStack(UUserWidget* Widget);
	

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ShowWidget(UUserWidget* Widget);

	UPROPERTY()
	UUserWidgetStack* WidgetStack;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UFadeUW> FadeWidgetClass;

	UPROPERTY()
	UFadeUW* FadeWidget;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UMainMenuUserWidget> MainMenuWidgetClass;

	UPROPERTY()
	UMainMenuUserWidget* MainMenuWidget;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<USelectLevelUserWidget> SelectLevelWidgetClass;

	UPROPERTY()
	USelectLevelUserWidget* SelectLevelWidget;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UStoryUW> StoryWidgetClass;

	UPROPERTY()
	UStoryUW* StoryWidget;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<USettingsUW> SettingsWidgetClass;

	UPROPERTY()
	USettingsUW* SettingsWidget;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UCreditsUW> CreditsWidgetClass;

	UPROPERTY()
	UCreditsUW* CreditsWidget;


	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UControlsUW> ControlsWidgetClass;

	UPROPERTY()
	UControlsUW* ControlsWidget;
};
