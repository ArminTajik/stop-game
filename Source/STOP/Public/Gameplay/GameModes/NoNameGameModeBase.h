// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/GameRulesDataAsset.h"
#include "GameFramework/GameModeBase.h"
#include "NoNameGameModeBase.generated.h"

USTRUCT(BlueprintType)
struct FLevelData
{
	GENERATED_BODY()
	int32 LevelNum;
	bool bDidWin;
	int32 Score;
	bool bLastLevel;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterEvent, APassengerCharacterBase*, Character);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrapdoorChangeActive, bool, bIsActive);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOver, FLevelData, LevelData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPauseGame, bool, bIsPaused);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerUpUsed, EPowerUp, PowerUpType);


UCLASS(minimalapi)
class ANoNameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANoNameGameModeBase();
	bool InitGameRules();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UFUNCTION(BlueprintCallable, Category = "Game Stats")
	void RestartLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Game Stats")
	void UnlockNextLevel();


	// Delegates ==========================================================
	// Broadcast when a character is scanned
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterEvent OnCharacterEntered;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterEvent OnCharacterExited;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterEvent OnCharacterInteracted;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterEvent OnCharacterScanned;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterEvent OnCharacterDestroyed;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTrapdoorChangeActive OnTrapdoorChangeActive;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameOver OnGameOver;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPauseGame OnPauseGame;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPowerUpUsed OnPowerUpUsed;


	// Properties ==========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
	UGameRulesDataAsset* DefaultGameRules;

	UPROPERTY(BlueprintReadWrite, Category = "Game Rules")
	UGameRulesDataAsset* ActiveGameRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
	class ULevelListDataAsset* LevelListDataAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bIsGameOver = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentLevelIndex;


	// Functions ==========================================================
	
	UFUNCTION()
	void QuitToMainMenu() const;

	UFUNCTION()
	void PauseGame() const;

	UFUNCTION()
	void ResumeGame() const;

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void GameOver(FLevelData LevelData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
