// Copyright Epic Games, Inc. All Rights Reserved.
#include "Gameplay/GameModes/MainMenuGameMode.h"
#include "Blueprint/UserWidget.h"
#include "UIComponents/Menus/MainMenuUserWidget.h"
#include "UObject/ConstructorHelpers.h"

AMainMenuGameMode::AMainMenuGameMode()
{
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UMainMenuUserWidget>(GetWorld(), MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
		}
	}
}

