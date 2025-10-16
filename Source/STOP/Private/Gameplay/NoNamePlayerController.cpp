// Copyright Epic Games, Inc. All Rights Reserved.

#include "Gameplay/NoNamePlayerController.h"

#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Actors/EnterDoorActor.h"
#include "Blueprint/UserWidget.h"
#include "Characters/PlayerPawn.h"
#include "Components/StatsManagerComponent.h"
#include "DataPersistance/DataPersistenceManager.h"
#include "Engine/LocalPlayer.h"
#include "Gameplay/NoNameGameInstance.h"
#include "Gameplay/GameModes/NoNameGameModeBase.h"
#include "Interfaces/HoldableInterface.h"
#include "Interfaces/InteractableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UIComponents/Menus/GameOverUserWidget.h"
#include "UIComponents/HUDUserWidget.h"
#include "UIComponents/Menus/PauseMenuUserWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ANoNamePlayerController::ANoNamePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;


	StatsManager = CreateDefaultSubobject<UStatsManagerComponent>(TEXT("StatsManager"));
	UE_LOG(LogPlayerController, Warning, TEXT("StatsManager initialized."));
}

void ANoNamePlayerController::BeginPlay()
{
	UE_LOG(LogPlayerController, Log, TEXT("PlayerController class: %s"), *GetName());
	// Call the base class  
	Super::BeginPlay();

	SetupUI();
	BindToGameMode();
	SetupStatsManager();
	SetupLevelTime();
}

#pragma region Setup =================================================
void ANoNamePlayerController::SetupUI()
{
	if (GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UGameOverUserWidget>(GetWorld(), GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
			GameOverWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuUserWidget>(GetWorld(), PauseMenuWidgetClass);
		if (PauseMenuWidget)
		{
			PauseMenuWidget->AddToViewport();
			PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (HUDWidgetClass)
	{
		// Create and add the HUD widget to the viewport
		HUDWidget = CreateWidget<UHUDUserWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

	// make sure the player can interact with the UI and the game
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetWidgetToFocus(nullptr);

	SetInputMode(InputMode);
	bShowMouseCursor = true;

	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ANoNamePlayerController::BindToGameMode()
{
	if (ANoNameGameModeBase* GameMode = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		ActiveGameRules = GameMode->ActiveGameRules;
		GameMode->OnGameOver.AddDynamic(this, &ANoNamePlayerController::OnGameOver);
		GameMode->OnCharacterEntered.AddDynamic(this, &ANoNamePlayerController::OnCharacterEnter);
		GameMode->OnCharacterExited.AddDynamic(this, &ANoNamePlayerController::OnCharacterExit);
		GameMode->OnCharacterInteracted.AddDynamic(this, &ANoNamePlayerController::OnInteractWithPassenger);
		GameMode->OnCharacterScanned.AddDynamic(this, &ANoNamePlayerController::OnCharacterScanned);
		GameMode->OnCharacterDestroyed.AddDynamic(this, &ANoNamePlayerController::OnCharacterDestroyed);
		GameMode->OnPauseGame.AddDynamic(this, &ANoNamePlayerController::OnSetPauseGame);
		GameMode->OnPowerUpUsed.AddDynamic(this, &ANoNamePlayerController::OnPowerUpUsed);

		if (HUDWidget)
		{
			UE_LOG(LogPlayerController, Log, TEXT("HUDWidget created."));
			if (UNoNameGameInstance* GI = GetGameInstance<UNoNameGameInstance>())
			{
				HUDWidget->AddToDebugText(FString::Printf(TEXT("Level: %s"), *GI->GetCleanLevelName()));
				HUDWidget->AddToDebugText(FString::Printf(TEXT("Level Num: %d"), GI->CurrentLevelIndex));
			}
			HUDWidget->AddToDebugText(FString::Printf(TEXT("Game Mode: %s"), *GetNameSafe(GameMode)));
			HUDWidget->AddToDebugText(FString::Printf(TEXT("Game Rules: %s"), *GetNameSafe(ActiveGameRules)));
		}
	}
	else
	{
		UE_LOG(LogPlayerController, Error, TEXT("Failed to get game mode!"));
	}
}

void ANoNamePlayerController::SetupStatsManager()
{
	if (StatsManager)
	{
		StatsManager->ScorePopupActorClass = ScorePopupActorClass;

		StatsManager->OnScoreChanged.AddDynamic(this, &ANoNamePlayerController::OnScoreChanged);
		StatsManager->OnHealthChanged.AddDynamic(this, &ANoNamePlayerController::OnHealthChanged);
		StatsManager->OnComboChanged.AddDynamic(this, &ANoNamePlayerController::OnComboChanged);

		FGameStats InitGameStats = {
			.CurrentHealth = 0,
			.CurrentScore = 0,
			.HighScore = 0
		};

		if (UDataPersistenceManager* DPM = GetGameInstance()->GetSubsystem<UDataPersistenceManager>())
		{
			InitGameStats.HighScore = DPM->GetHighestScore();
			InitGameStats.CurrentScore = InitGameStats.HighScore;
			if (IsValid(ActiveGameRules))
			{
				InitGameStats.CurrentHealth = ActiveGameRules->InitialHealth;
			}
		}

		StatsManager->Init(InitGameStats);
		if (HUDWidget)
		{
			HUDWidget->UpdateHealth(StatsManager->GetHealth());
			HUDWidget->UpdateScore(StatsManager->GetScore());
		}
	}
	else
	{
		UE_LOG(LogPlayerController, Warning, TEXT("StatsManager is not initialized!"));
	}
}

void ANoNamePlayerController::SetupLevelTime()
{
	if (ActiveGameRules)
	{
		// LevelMaxTime = ActiveGameRules->LevelDuration;
		LevelTimer = 0;
		UE_LOG(LogGameMode, Log, TEXT("Level Timer set to: %f seconds"), LevelTimer);
	}
	else
	{
		UE_LOG(LogGameMode, Warning, TEXT("No Active Game Rules found! Level Timer not set."));
	}
	GetWorldTimerManager().SetTimer(
		TimeUpdateTimerHandle,
		this,
		&ANoNamePlayerController::UpdateLevelTime,
		TimeUpdateInterval,
		true);
}

void ANoNamePlayerController::AddTrapdoorButton(ATrapdoorActor* TrapdoorActor)
{
	if (HUDWidget)
	{
		HUDWidget->AddTrapdoorButton(TrapdoorActor);
	}
	else
	{
		UE_LOG(LogPlayerController, Warning, TEXT("HUDWidget is not initialized! Cannot add trapdoor button."));
	}
}
#pragma endregion
#pragma region Input Handlres =========================================================

void ANoNamePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this,
		                                   &ANoNamePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this,
		                                   &ANoNamePlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this,
		                                   &ANoNamePlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this,
		                                   &ANoNamePlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this,
		                                   &ANoNamePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this,
		                                   &ANoNamePlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this,
		                                   &ANoNamePlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this,
		                                   &ANoNamePlayerController::OnTouchReleased);

		// Setup fastforward input events
		EnhancedInputComponent->BindAction(FastForwardAction, ETriggerEvent::Started, this,
		                                   &ANoNamePlayerController::OnFastForwardStarted);
		EnhancedInputComponent->BindAction(FastForwardAction, ETriggerEvent::Completed, this,
		                                   &ANoNamePlayerController::OnFastForwardReleased);
		EnhancedInputComponent->BindAction(FastForwardAction, ETriggerEvent::Canceled, this,
		                                   &ANoNamePlayerController::OnFastForwardReleased);
	}
	else
	{
		UE_LOG(LogPlayerController, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void ANoNamePlayerController::OnInputStarted()
{
	HitResult.Reset();
	bIsRotating = true;

	// EnterCameraDragMode();

#if PLATFORM_ANDROID || PLATFORM_IOS
	float TouchX, TouchY;
	bool bIsPressed = false;
	GetInputTouchState(ETouchIndex::Touch1, TouchX, TouchY, bIsPressed);
	if (bIsPressed)
	{
		InitialTouchPosition = FVector2D(TouchX, TouchY);
	}
#else
	GetMousePosition(InitialTouchPosition.X, InitialTouchPosition.Y);
#endif

	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, false, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
		HitResult = Hit;

		if (HitResult.IsValidBlockingHit())
		{
			if (AActor* HitActor = HitResult.GetActor(); HitActor && HitActor->GetClass()->ImplementsInterface(
				UHoldableInterface::StaticClass()))
			{
				bIsRotating = false;
			}
		}
	}
}

// Triggered every frame when the input is held down
void ANoNamePlayerController::OnSetDestinationTriggered()
{
	FollowTime += GetWorld()->GetDeltaSeconds();

	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, false, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
		HitResult = Hit;

		if (!bIsHolding && HitResult.IsValidBlockingHit())
		{
			UE_LOG(LogPlayerController, Log, TEXT("Hit Location: %s on component: %s on actor: %s"),
			       *HitResult.Location.ToString(),
			       *GetNameSafe(HitResult.GetComponent()),
			       *GetNameSafe(HitResult.GetActor()));
			// Interact with the object if it is interactable
			if (AActor* HitActor = HitResult.GetActor(); HitActor && HitActor->GetClass()->ImplementsInterface(
				UHoldableInterface::StaticClass()))
			{
				bIsHolding = true;
				IHoldableInterface::Execute_OnHoldStart(HitActor, this);
			}
		}
	}


	if (bIsRotating)
	{
		FVector2D CurrentInputPosition;

#if PLATFORM_ANDROID || PLATFORM_IOS
		float TouchX, TouchY;
		bool bIsPressed = false;
		GetInputTouchState(ETouchIndex::Touch1, TouchX, TouchY, bIsPressed);
		if (!bIsPressed) return;

		CurrentInputPosition = FVector2D(TouchX, TouchY);
#else
		if (!GetMousePosition(CurrentInputPosition.X, CurrentInputPosition.Y)) return;
#endif

		const float DeltaX = CurrentInputPosition.X - InitialTouchPosition.X;
		const float DeltaY = CurrentInputPosition.Y - InitialTouchPosition.Y;

		if (APawn* ControlledPawn = GetPawn())
		{
			if (APlayerPawn* PlayerPawn = Cast<APlayerPawn>(ControlledPawn))
			{
				PlayerPawn->UpdateCameraOrthographicSize(DeltaY * 0.01f);
				PlayerPawn->UpdateCameraRotation(DeltaX * 0.1f);
				// Adjust camera zoom based on vertical movement
			}
		}

		InitialTouchPosition = CurrentInputPosition;
	}
}

void ANoNamePlayerController::OnSetDestinationReleased()
{
	UE_LOG(LogPlayerController, Log, TEXT("Input Released"));
	bIsRotating = false;

	if (HitResult.IsValidBlockingHit())
	{
		UE_LOG(LogPlayerController, Log, TEXT("Hit Location: %s on component: %s on actor: %s"),
		       *HitResult.Location.ToString(),
		       *GetNameSafe(HitResult.GetComponent()),
		       *GetNameSafe(HitResult.GetActor()));
		// Interact with the object if it is interactable

		if (AActor* HitActor = HitResult.GetActor(); IsValid(HitActor))
		{
			if (bIsHolding && HitActor->GetClass()->ImplementsInterface(UHoldableInterface::StaticClass()))
			{
				bIsHolding = false;
				IHoldableInterface::Execute_OnHoldEnd(HitActor, this);
			}
			if (HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			{
				IInteractableInterface::Execute_OnInteract(HitActor, this);
			}
		}
	}
}

// Triggered every frame when the input is held down
void ANoNamePlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ANoNamePlayerController::OnTouchReleased()
{
	UE_LOG(LogPlayerController, Log, TEXT("Touch Released"));
	bIsTouch = false;
	OnSetDestinationReleased();
}

void ANoNamePlayerController::OnFastForwardStarted()
{
}

void ANoNamePlayerController::OnFastForwardReleased()
{
}
#pragma endregion
#pragma region Stats Handlers =========================================================

void ANoNamePlayerController::UpdateLevelTime()
{
	LevelTimer += TimeUpdateInterval;
	UpdateTimeUI(LevelTimer);
}


void ANoNamePlayerController::OnScoreChanged(int32 NewScore)
{
	UE_LOG(LogPlayerController, Log, TEXT("Health changed: %d"), NewScore);
	if (bIsGameOver)
	{
		UE_LOG(LogPlayerController, Warning, TEXT("Game is already over. Cannot add score."));
		return;
	}
	if (ANoNameGameModeBase* GM = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (!IsValid(ActiveGameRules))
		{
			UE_LOG(LogPlayerController, Warning, TEXT("No Active Game Rules found! Cannot check for max score."));
			return;
		}

		if (NewScore >= ActiveGameRules->MaxScore && GameOverWidget && StatsManager)
		{
			UE_LOG(LogGameMode, Log, TEXT("Max score reached! Game Over!"));

			const FLevelData LevelData = {
				.bDidWin = true,
				.Score = NewScore,
			};

			GM->GameOver(LevelData);
		}
	}
	else
	{
		UE_LOG(LogPlayerController, Error, TEXT("Game Mode not found! Cannot trigger game over."));
	}
	if (HUDWidget)
	{
		HUDWidget->UpdateScore(NewScore);
	}
}

void ANoNamePlayerController::OnHealthChanged(int32 NewHealth)
{
	UE_LOG(LogPlayerController, Log, TEXT("Health changed: %d"), NewHealth);
	if (bIsGameOver)
	{
		UE_LOG(LogPlayerController, Warning, TEXT("Game is already over. Cannot change health."));
		return;
	}

	if (NewHealth <= 0 && GameOverWidget && StatsManager)
	{
		if (ANoNameGameModeBase* GM = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			UE_LOG(LogGameMode, Log, TEXT("Health reached zero! Game Over!"));

			const FLevelData LevelData = {
				.LevelNum = GM->CurrentLevelIndex,
				.bDidWin = false,
				.Score = StatsManager->GetScore()
			};

			GM->GameOver(LevelData);
		}
		else
		{
			UE_LOG(LogPlayerController, Error, TEXT("Game Mode not found! Cannot trigger game over."));
		}
		return;
	}

	if (HUDWidget)
	{
		HUDWidget->UpdateHealth(NewHealth);
	}
}

void ANoNamePlayerController::OnComboChanged(int32 NewCombo)
{
	UE_LOG(LogPlayerController, Log, TEXT("Combo changed: %d"), NewCombo);
	TryGainComboPowerUp(NewCombo);
}
#pragma endregion
#pragma region Character Event Handlers ================================================
void ANoNamePlayerController::OnCharacterExit(APassengerCharacterBase* PassengerCharacterBase)
{
	UE_LOG(LogGameMode, Log, TEXT("Passenger exited: %s"), *GetNameSafe(PassengerCharacterBase));
	if (!PassengerCharacterBase) return;
	if (StatsManager)
	{
		StatsManager->ApplyEvent(PassengerCharacterBase, ECharacterEvent::Exit);
	}
	TryGainPowerUp(PassengerCharacterBase);
	PassengerCharacterBase->Destroy();
}

void ANoNamePlayerController::OnCharacterEnter(APassengerCharacterBase* PassengerCharacterBase)
{
	UE_LOG(LogGameMode, Log, TEXT("Passenger entered: %s"), *GetNameSafe(PassengerCharacterBase));
}

void ANoNamePlayerController::OnInteractWithPassenger(APassengerCharacterBase* PassengerCharacterBase)
{
	if (!PassengerCharacterBase) return;
	if (StatsManager)
	{
		StatsManager->ApplyEvent(PassengerCharacterBase, ECharacterEvent::Interact);
	}
}

void ANoNamePlayerController::OnCharacterScanned(APassengerCharacterBase* PassengerCharacterBase)
{
	if (!PassengerCharacterBase) return;
	if (StatsManager)
	{
		StatsManager->ApplyEvent(PassengerCharacterBase, ECharacterEvent::Scan);
	}
}

void ANoNamePlayerController::OnCharacterDestroyed(APassengerCharacterBase* PassengerCharacterBase)
{
	if (!PassengerCharacterBase) return;
	if (StatsManager)
	{
		StatsManager->ApplyEvent(PassengerCharacterBase, ECharacterEvent::Destroy);
	}
	PassengerCharacterBase->Destroy();
}

void ANoNamePlayerController::StepStop()
{
	OnPassengerStop.Broadcast();
	GetWorldTimerManager().ClearTimer(StepWalkTimerHandle);
	GetWorldTimerManager().SetTimer(StepWalkTimerHandle, this, &ANoNamePlayerController::StepWalk, StepStopTime,
	                                false);
}


void ANoNamePlayerController::StepWalk()
{
	OnPassengerWalk.Broadcast();
	GetWorldTimerManager().ClearTimer(StepStopTimerHandle);
	GetWorldTimerManager().SetTimer(StepStopTimerHandle, this, &ANoNamePlayerController::StepStop, StepWalkTime,
	                                false);
}
#pragma endregion
#pragma region Game Event Handlers ================================================
void ANoNamePlayerController::OnPowerUpUsed(const EPowerUp PowerUpType)
{
	switch (PowerUpType)
	{
	case EPowerUp::EliminateAllAliens:
		HandleMoveEliminateAllAliens();
		UE_LOG(LogPlayerController, Log, TEXT("Special Move: Eliminate All Aliens applied!"));
		break;
	case EPowerUp::RevealAllCharacters:
		HandleMoveRevealAllCharacters();
		UE_LOG(LogPlayerController, Log, TEXT("Special Move: Reveal All Characters applied!"));
		break;
	case EPowerUp::StopRandomSpawner:
		HandleMoveStopRandomSpawner();
		UE_LOG(LogPlayerController, Log, TEXT("Special Move: Stop Random Spawner applied!"));
		break;
	default:
		UE_LOG(LogPlayerController, Warning, TEXT("Unknown special move type: %s"),
		       *UEnum::GetValueAsString(PowerUpType));
		break;
	}
}

void ANoNamePlayerController::OnGameOver(const FLevelData LevelData)
{
	bIsGameOver = true;

	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
	}
	GameOverWidget->SetVisibility(ESlateVisibility::Visible);

	GameOverWidget->GameOver(LevelData);
}

void ANoNamePlayerController::OnSetPauseGame(bool bIsPaused)
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->SetVisibility(bIsPaused ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	else
	{
		UE_LOG(LogPlayerController, Warning, TEXT("PauseMenuWidget is not initialized!"));
	}

	if (HUDWidget)
	{
		HUDWidget->SetVisibility(bIsPaused ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogPlayerController, Warning, TEXT("HUDWidget is not initialized!"));
	}
}

void ANoNamePlayerController::UpdateTimeUI(float NewTime)
{
	if (HUDWidget)
	{
		// Update the HUD with the current game time
		HUDWidget->UpdateTime(NewTime);
	}
}
#pragma endregion
#pragma region PowerUps =========================================================

void ANoNamePlayerController::TryGainPowerUp(APassengerCharacterBase* Passenger)
{
	if (!ActiveGameRules)
	{
		UE_LOG(LogPlayerController, Warning, TEXT("ActiveGameRules is not set! Cannot apply power up."));
		return;
	}

	for (const auto& [CharacterTag, MoveType] : ActiveGameRules->CharacterPowerUps)
	{
		if (Passenger->CharacterType.Tags.HasTag(CharacterTag))
		{
			if (HUDWidget)
			{
				HUDWidget->SetPowerUp(MoveType);
			}
			UE_LOG(LogPlayerController, Log, TEXT("Power up %s granted for character: %s"),
			       *UEnum::GetValueAsString(MoveType), *GetNameSafe(Passenger));
			break;
		}
	}
}

void ANoNamePlayerController::TryGainComboPowerUp(int32 Combo)
{
	if (!ActiveGameRules)
	{
		UE_LOG(LogPlayerController, Warning, TEXT("ActiveGameRules is not set! Cannot apply power up."));
		return;
	}

	for (const auto& [ComboAmount, PowerUpType] : ActiveGameRules->ComboPowerUps)
	{
		if (Combo == ComboAmount)
		{
			if (HUDWidget)
			{
				HUDWidget->SetPowerUp(PowerUpType);
			}
			UE_LOG(LogPlayerController, Log, TEXT("Power up %s granted for combo of %d"),
			       *UEnum::GetValueAsString(PowerUpType), Combo);
			break;
		}
	}
}

void ANoNamePlayerController::HandleMoveEliminateAllAliens()
{
	for (TActorIterator<APassengerCharacterBase> It(GetWorld()); It; ++It)
	{
		APassengerCharacterBase* Passenger = *It;
		if (Passenger && Passenger->CharacterType.Tags.HasTag(
			FGameplayTag::RequestGameplayTag(FName("Character.Alien"))))
		{
			// If the passenger is an alien, destroy it
			UE_LOG(LogGameMode, Log, TEXT("Destroying alien character: %s"),
			       *GetNameSafe(Passenger));
			if (Passenger->IsPendingKillPending())
			{
				UE_LOG(LogGameMode, Warning, TEXT("Passenger is already pending kill: %s"),
				       *GetNameSafe(Passenger));
				continue;
			}

			// Apply stats event before destroying
			if (StatsManager)
			{
				StatsManager->ApplyEvent(Passenger, ECharacterEvent::Destroy);
			}
			Passenger->Destroy();
			UE_LOG(LogGameMode, Log, TEXT("Alien character destroyed: %s"), *GetNameSafe(Passenger));
		}
	}
}

void ANoNamePlayerController::HandleMoveRevealAllCharacters()
{
	for (TActorIterator<APassengerCharacterBase> It(GetWorld()); It; ++It)
	{
		APassengerCharacterBase* Passenger = *It;
		// If the passenger is hidden, reveal it
		UE_LOG(LogGameMode, Log, TEXT("Revealing character: %s"), *GetNameSafe(Passenger));
		Passenger->RevealCharacter();
	}
}

void ANoNamePlayerController::HandleMoveStopRandomSpawner()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnterDoorActor::StaticClass(), FoundActors);

	// pick spawner actor in the world randomly
	AEnterDoorActor* Spawner =
		FoundActors.Num() > 0
			? Cast<AEnterDoorActor>(FoundActors[FMath::RandRange(0, FoundActors.Num() - 1)])
			: nullptr;

	if (Spawner)
	{
		// If the spawner is a random spawner, stop it
		UE_LOG(LogGameMode, Log, TEXT("Stopping random spawner: %s"), *GetNameSafe(Spawner));
		Spawner->StopSpawning();
	}
}
#pragma endregion
