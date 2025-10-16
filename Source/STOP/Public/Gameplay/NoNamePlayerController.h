// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/GameRulesDataAsset.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "NoNamePlayerController.generated.h"

struct FLevelData;
class AScorePopupActor;
class UGameRulesDataAsset;
class ATrapdoorActor;
/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPassengerStop);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPassengerWalk);

UCLASS()
class ANoNamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANoNamePlayerController();
	void SetupUI();
	void BindToGameMode();
	UFUNCTION()
	void SetupStatsManager();
	void SetupLevelTime();
	void AddTrapdoorButton(ATrapdoorActor* TrapdoorActor);


	UPROPERTY(EditAnywhere, Category="Stepping", meta=(ClampMin="0.01"))
	float StepStopTime = 0.1f;

	UPROPERTY(EditAnywhere, Category="Stepping", meta=(ClampMin="0.01"))
	float StepWalkTime = 0.5f;

	UPROPERTY()
	FTimerHandle StepStopTimerHandle;

	UPROPERTY()
	FTimerHandle StepWalkTimerHandle;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPassengerStop OnPassengerStop;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPassengerWalk OnPassengerWalk;


	UFUNCTION()
	void TryGainPowerUp(APassengerCharacterBase* Passenger);

	UFUNCTION()
	void TryGainComboPowerUp(int32 Combo);

	UFUNCTION()
	void HandleMoveEliminateAllAliens();

	UFUNCTION()
	void HandleMoveRevealAllCharacters();

	UFUNCTION()
	void HandleMoveStopRandomSpawner();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump's Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	/** Jump's Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	/** FastForward Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FastForwardAction;

	UPROPERTY(BlueprintReadWrite, Category = "Game Rules")
	UGameRulesDataAsset* ActiveGameRules;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStatsManagerComponent* StatsManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<AScorePopupActor> ScorePopupActorClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bIsGameOver = false;
	// Functions ==================================================

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;

	void StepWalk();
	void StepStop();

	// To add mapping context
	virtual void BeginPlay() override;


	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

	void OnFastForwardStarted();
	void OnFastForwardReleased();


	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual void OnCharacterExit(APassengerCharacterBase* PassengerCharacterBase);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual void OnCharacterEnter(APassengerCharacterBase* PassengerCharacterBase);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual void OnInteractWithPassenger(APassengerCharacterBase* PassengerCharacterBase);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual void OnCharacterScanned(APassengerCharacterBase* PassengerCharacterBase);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual void OnCharacterDestroyed(APassengerCharacterBase* PassengerCharacterBase);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void OnPowerUpUsed(EPowerUp PowerUpType);

	UPROPERTY()
	FTimerHandle TimeUpdateTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float TimeUpdateInterval = 0.1f;
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void UpdateLevelTime();

private:
	// Properties for UI ==================================================
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UHUDUserWidget> HUDWidgetClass;

	UPROPERTY()
	UHUDUserWidget* HUDWidget = nullptr;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UGameOverUserWidget> GameOverWidgetClass;

	UPROPERTY()
	UGameOverUserWidget* GameOverWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UPauseMenuUserWidget> PauseMenuWidgetClass;

	UPROPERTY()
	UPauseMenuUserWidget* PauseMenuWidget = nullptr;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnSetPauseGame(bool bIsPaused);

	UFUNCTION()
	void UpdateTimeUI(float NewTime);

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void OnGameOver(FLevelData LevelData);

	UFUNCTION()
	void OnScoreChanged(int32 NewScore);

	UFUNCTION()
	void OnHealthChanged(int32 NewHealth);
	
	UFUNCTION()
	void OnComboChanged(int32 NewCombo);


	// Properties for input handling ==================================================
	FVector CachedDestination;
	FHitResult HitResult; // Hit result for the mouse click

	bool bIsTouch; // Is it a touch device

	bool bIsRotating = false; // Is the camera rotating

	bool bIsHolding; // Is the player holding down some holdable input

	FVector2D InitialTouchPosition; // Initial touch position when the input started

	float FollowTime; // For how long it has been pressed

private:
	float LevelTimer = 0.0f;
	float LevelMaxTime = 300.0f; // Default max time for the level in seconds
};
