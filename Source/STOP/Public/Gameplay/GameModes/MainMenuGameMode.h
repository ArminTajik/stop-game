// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

UCLASS(minimalapi)
class AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainMenuGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

protected:
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UMainMenuUserWidget> MainMenuWidgetClass;

	UPROPERTY()
	UMainMenuUserWidget* MainMenuWidget;
};
