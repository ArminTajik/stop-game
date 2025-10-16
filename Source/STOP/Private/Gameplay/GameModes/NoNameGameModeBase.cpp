// Copyright Epic Games, Inc. All Rights Reserved.

#include "Gameplay/GameModes/NoNameGameModeBase.h"
#include "DataAssets/LevelListDataAsset.h"
#include "DataPersistance/DataPersistenceManager.h"
#include "Gameplay/NoNameGameInstance.h"
#include "Gameplay/NoNamePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ANoNameGameModeBase::ANoNameGameModeBase()
{
	// use our custom PlayerController class
	PlayerControllerClass = ANoNamePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/Blueprints/Characters/BP_PlayerPawn"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(
		TEXT("/Game/Blueprints/Gameplay/PlayerController/BP_NoNamePlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}

void ANoNameGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	if (!InitGameRules())
	{
		ErrorMessage = FString::Printf(TEXT("Failed to initialize game rules for %s"), *GetName());
		UE_LOG(LogGameMode, Error, TEXT("%s"), *ErrorMessage);
	}

	if (UNoNameGameInstance* GI = GetGameInstance<UNoNameGameInstance>())
	{
		CurrentLevelIndex = GI->CurrentLevelIndex;
		UE_LOG(LogGameMode, Log, TEXT("Current Level Number: %d"), CurrentLevelIndex);
	}
}


void ANoNameGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

bool ANoNameGameModeBase::InitGameRules()
{
	// Game Rules Initialization
	if (UNoNameGameInstance* GI = GetGameInstance<UNoNameGameInstance>())
	{
		if (GI->TryGetGameRules(ActiveGameRules))
		{
			UE_LOG(LogGameMode, Log, TEXT("Game Rules assigned: %s"),
			       *GetClass()->GetName(), *ActiveGameRules->GetName());
			return true;
		}

		if (GI->TryGetGameRulesFromLevelList(ActiveGameRules, LevelListDataAsset))
		{
			UE_LOG(LogGameMode, Log, TEXT("Game Rules assigned from Level List: %s"), *ActiveGameRules->GetName());
			return true;
		}
	}

	if (DefaultGameRules)
	{
		UE_LOG(LogGameMode, Warning, TEXT("No Active Game Rules found, using DefaultGameRules: %s"),
		       *DefaultGameRules->GetName());
		ActiveGameRules = DefaultGameRules;
		return true;
	}

	// No Game Rules found, log an error

	UE_LOG(LogGameMode, Error,
	       TEXT("No Game Rules found! Please set DefaultGameRules or ensure the level has Game Rules assigned."));

	return false;
}


// Menu UI METHODS ===========================================================

void ANoNameGameModeBase::QuitToMainMenu() const
{
	if (GetWorld())
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"), true);
	}
	else
	{
		UE_LOG(LogGameMode, Warning, TEXT("World is not valid! Cannot quit to main menu."));
	}
}

void ANoNameGameModeBase::PauseGame() const
{
	if (GetWorld())
	{
		GetWorld()->GetWorldSettings()->SetTimeDilation(0.0f);
	}
	else
	{
		UE_LOG(LogGameMode, Warning, TEXT("World is not valid! Cannot pause game."));
	}

	OnPauseGame.Broadcast(true);
}

void ANoNameGameModeBase::ResumeGame() const
{
	if (GetWorld())
	{
		GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
	}
	else
	{
		UE_LOG(LogGameMode, Warning, TEXT("World is not valid! Cannot resume game."));
	}

	OnPauseGame.Broadcast(false);
}


void ANoNameGameModeBase::RestartLevel() const
{
	if (GetWorld())
	{
		GetWorld()->ServerTravel(GetWorld()->GetMapName() + "?restart");
	}
	else
	{
		UE_LOG(LogGameMode, Error, TEXT("Failed to restart level, world is null!"));
	}
}

void ANoNameGameModeBase::UnlockNextLevel()
{
	if (LevelListDataAsset)
	{
		const int LevelIndexToUnlock = CurrentLevelIndex + 1;
		if (LevelIndexToUnlock >= LevelListDataAsset->Levels.Num())
		{
			UE_LOG(LogGameMode, Warning, TEXT("No more levels to unlock! Current level index: %d"),
			       LevelIndexToUnlock);
			return;
		}
		if (UDataPersistenceManager* DPM = GetGameInstance()->GetSubsystem<UDataPersistenceManager>())
		{
			if (DPM->GetHighestUnlockedLevel() >= LevelIndexToUnlock)
			{
				UE_LOG(LogGameMode, Log, TEXT("Level index %d is already unlocked."), LevelIndexToUnlock);
				return;
			}

			UE_LOG(LogGameMode, Log, TEXT("Unlocking level index: %d"), LevelIndexToUnlock);
			DPM->LevelCompleted(LevelIndexToUnlock);
		}
	}
	else
	{
		UE_LOG(LogGameMode, Warning, TEXT("LevelListDataAsset is not set! Cannot unlock next level."));
	}
}

void ANoNameGameModeBase::GameOver(FLevelData LevelData)
{
	UE_LOG(LogGameMode, Log, TEXT("Game Over! Final Score: %d"), LevelData.Score);
	bIsGameOver = true;

	// set time dilation to 0 to pause the game
	LevelData.LevelNum = CurrentLevelIndex;

	// Define if it's the last level
	if (LevelListDataAsset && LevelListDataAsset->Levels.Num() > CurrentLevelIndex + 1)
	{
		LevelData.bLastLevel = false;
	}
	else
	{
		LevelData.bLastLevel = true;
	}

	OnGameOver.Broadcast(LevelData);
}
