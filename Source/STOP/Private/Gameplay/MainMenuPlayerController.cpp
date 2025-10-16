// Copyright Epic Games, Inc. All Rights Reserved.

#include "Gameplay/MainMenuPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/NoNameGameInstance.h"
#include "UIComponents/Elements/FadeUW.h"
#include "UIComponents/Menus/MainMenuUserWidget.h"
#include "UIComponents/Menus/SelectLevelUserWidget.h"
#include "UIComponents/Menus/SettingsUW.h"
#include "UIComponents/Helpers/UserWidgetStack.h"
#include "UIComponents/Menus/CreditsUW.h"
#include "UIComponents/Menus/ControlsUW.h"
#include "UIComponents/Menus/StoryUW.h"


DEFINE_LOG_CATEGORY(LogMainMenuPlayerController);

AMainMenuPlayerController::AMainMenuPlayerController()
{
}

void AMainMenuPlayerController::HandleStart(ULevelDataAsset* LevelToLoad)
{
	if (IsValid(FadeWidget))
	{
		FadeWidget->FadeInOut();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, LevelToLoad]()
	{
		if (WidgetStack)
		{
			WidgetStack->ClearUIStack();
		}

		// OnStartLevel.Broadcast();
		if (UNoNameGameInstance* GI = GetGameInstance<UNoNameGameInstance>())
		{
			GI->SetSelectedLevel(LevelToLoad);
			GI->StartGame();
		}
		else
		{
			UE_LOG(LogLevelSelectionMenu, Warning, TEXT("Failed to get Game Instance!"));
		}
	}, 0.15f, false);
}

void AMainMenuPlayerController::PushWidgetToStack(UUserWidget* Widget)
{
	if (Widget && WidgetStack)
	{
		WidgetStack->PushUI(Widget);
	}
}

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	WidgetStack = NewObject<UUserWidgetStack>(this);
	ensureAlwaysMsgf(WidgetStack, TEXT("WidgetStack failed to construct"));

	if (FadeWidgetClass)
	{
		FadeWidget = CreateWidget<UFadeUW>(this, FadeWidgetClass);
		if (IsValid(FadeWidget))
		{
			FadeWidget->AddToViewport(100); // High to make sure it's on top
			FadeWidget->FadeInOut();
		}
	}
	// Create and show the main menu widget
	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UMainMenuUserWidget>(this, MainMenuWidgetClass);
		if (IsValid(MainMenuWidget) && IsValid(WidgetStack))
		{
			WidgetStack->PushUI(MainMenuWidget);
		}
	}

	if (SelectLevelWidgetClass)
	{
		SelectLevelWidget = CreateWidget<USelectLevelUserWidget>(this, SelectLevelWidgetClass);
	}

	if (StoryWidgetClass)
	{
		StoryWidget = CreateWidget<UStoryUW>(this, StoryWidgetClass);
	}

	if (SettingsWidgetClass)
	{
		SettingsWidget = CreateWidget<USettingsUW>(this, SettingsWidgetClass);
	}

	if (CreditsWidgetClass)
	{
		CreditsWidget = CreateWidget<UCreditsUW>(this, CreditsWidgetClass);
	}
	if (ControlsWidgetClass)
	{
		ControlsWidget = CreateWidget<UControlsUW>(this, ControlsWidgetClass);
	}

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AMainMenuPlayerController::ShowWidget(UUserWidget* Widget)
{
	if (IsValid(FadeWidget))
	{
		FadeWidget->FadeInOut();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Widget]()
	{
		PushWidgetToStack(Widget);
	}, 0.15f, false);
}


void AMainMenuPlayerController::ShowLevelSelectionMenu()
{
	ShowWidget(SelectLevelWidget);
}

void AMainMenuPlayerController::ShowStoryMenu()
{
	ShowWidget(StoryWidget);
}

void AMainMenuPlayerController::ShowSettingsMenu()
{
	ShowWidget(SettingsWidget);
}

void AMainMenuPlayerController::ShowCreditsMenu()
{
	ShowWidget(CreditsWidget);
}

void AMainMenuPlayerController::ShowControlsMenu()
{
	ShowWidget(ControlsWidget);
}


void AMainMenuPlayerController::BackToMainMenu()
{
	if (!WidgetStack)
		return;

	WidgetStack->ClearUIStack();
	ShowWidget(MainMenuWidget);
}
