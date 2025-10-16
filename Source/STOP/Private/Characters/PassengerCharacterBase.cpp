

#include "Characters/PassengerCharacterBase.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "../../Public/Gameplay/GameModes/NoNameGameModeBase.h"
#include "Actors/ScannerActor.h"
#include "Characters/PassengerAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/NoNamePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystems/NpcManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogPassengerCharacter);

// Sets default values
APassengerCharacterBase::APassengerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = APassengerAIController::StaticClass();

	ScanOverlayMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ScanOverlayMesh"));
	ScanOverlayMesh->SetupAttachment(GetMesh());

	ScanOverlayMesh->SetVisibility(false);
	ScanOverlayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScanOverlayMesh->bCastDynamicShadow = false;

}

void APassengerCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ScanOverlayMesh && GetMesh())
	{
		ScanOverlayMesh->SetupAttachment(GetMesh());
		ScanOverlayMesh->SetLeaderPoseComponent(GetMesh());
	}
}

// Called when the game starts or when spawned
void APassengerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UNpcManagerSubsystem* Manager = GI->GetSubsystem<UNpcManagerSubsystem>())
		{
			Manager->RegisterNPC(this);
		}
	}
	if (ANoNameGameModeBase* GM = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		// Bind to the game mode's events
		GM->OnGameOver.AddDynamic(this, &APassengerCharacterBase::HandleGameOver);

		if (GM->ActiveGameRules)
		{
			// Use the values from the game rules if available
			WalkSpeed = GM->ActiveGameRules->InitialWalkSpeed;
			WalkSpeedUpdateInterval = GM->ActiveGameRules->WalkSpeedUpdateInterval;
			MaxWalkSpeed = GM->ActiveGameRules->MaxWalkSpeed;
			WalkSpeedIncrement = GM->ActiveGameRules->WalkSpeedIncrement;
		}
		else
		{
			// Fallback to default values
			WalkSpeed = DefaultInitialWalkSpeed;
			WalkSpeedUpdateInterval = DefaultWalkSpeedUpdateInterval;
			MaxWalkSpeed = DefaultMaxWalkSpeed;
			WalkSpeedIncrement = DefaultWalkSpeedIncrement;
		}
	}

	if (ANoNamePlayerController* PC = Cast<ANoNamePlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->OnPassengerStop.AddDynamic(this, &APassengerCharacterBase::StepStop);
		PC->OnPassengerWalk.AddDynamic(this, &APassengerCharacterBase::StepWalk);
	}
	if (EnterSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EnterSoundCue, GetActorLocation());
	}

	// Set the initial walk speed
	if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
	{
		CharMovement->MaxWalkSpeed = WalkSpeed;
	}

	// increase the walk speed over time
	GetWorldTimerManager().SetTimer(WalkSpeedUpdateTimerHandle, this, &APassengerCharacterBase::UpdateWalkSpeed,
	                                WalkSpeedUpdateInterval, true);
}

void APassengerCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UGameInstance* GI = GetGameInstance())
	{
		if (UNpcManagerSubsystem* Manager = GI->GetSubsystem<UNpcManagerSubsystem>())
		{
			Manager->UnregisterNPC(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void APassengerCharacterBase::MovePassenger(APassengerCharacterBase* PassengerCharacterBase,
                                            const FVector& TargetLocation)
{
	if (AController* PController = PassengerCharacterBase->GetController())
	{
		UE_LOG(LogPassengerCharacter, Log, TEXT(" Moving passenger to location: %s"),
		       *TargetLocation.ToString());
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PController, TargetLocation);
		bIsMoving = true;
	}
}

void APassengerCharacterBase::StepStop()
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
		bIsMoving = false;
	}
}

void APassengerCharacterBase::StepWalk()
{
	MovePassenger(this, DestinationLocation);
}

void APassengerCharacterBase::UpdateWalkSpeed()
{
	if (!bIsMoving || bIsWalkSpeedAdjusting) return;

	if (WalkSpeed < MaxWalkSpeed)
	{
		WalkSpeed = FMath::Min(MaxWalkSpeed, WalkSpeed + WalkSpeedIncrement);
		if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
		{
			CharMovement->MaxWalkSpeed = WalkSpeed;
		}
	}
}

// Called to bind functionality to input
void APassengerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void APassengerCharacterBase::InitializeMovement(const FVector& Destination)
{
	// set the destination location to the end of the spline
	DestinationLocation = Destination;
	SpawnTime = FPlatformTime::Seconds();

	if (APassengerAIController* PController = Cast<APassengerAIController>(this->GetController()))
	{
		UE_LOG(LogPassengerCharacter, Log, TEXT("Initializing movement to destination: %s"),
		       *DestinationLocation.ToString());
		PController->SetDestination(DestinationLocation);
	}
}

void APassengerCharacterBase::Exit()
{
	if (ANoNameGameModeBase* GM = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnCharacterExited.Broadcast(this);
	}

	if (ExitSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExitSoundCue, GetActorLocation());
	}
}

void APassengerCharacterBase::CoverCharacter()
{
	bRevealed = false;
	HideScanned();

	// clear the timer to reveal the character
	GetWorldTimerManager().ClearTimer(RevealTimerHandle);
}


void APassengerCharacterBase::RevealCharacter()
{
	bRevealed = true;
	ShowScanned();

	// set timer to hide the character after a delay
	GetWorldTimerManager().SetTimer(RevealTimerHandle, this, &APassengerCharacterBase::CoverCharacter, 5.0f, false);
}

void APassengerCharacterBase::ShowScanned()
{
	if (USkeletalMeshComponent* CachedMesh = GetMesh(); IsValid(CachedMesh) && ScanOverlayMesh)
	{
		ScanOverlayMesh->SetVisibility(true);
		CachedMesh->SetVisibility(false);
	}
}

void APassengerCharacterBase::HideScanned()
{
	if (USkeletalMeshComponent* CachedMesh = GetMesh(); IsValid(CachedMesh) && ScanOverlayMesh)
	{
		ScanOverlayMesh->SetVisibility(false);
		CachedMesh->SetVisibility(true);
	}
}

void APassengerCharacterBase::SetSpawnerID(const int32 ID)
{
	SpawnerID = ID;
	UE_LOG(LogPassengerCharacter, Log, TEXT("Setting SpawnerID for passenger %s to %d"), *GetNameSafe(this), SpawnerID);
}

void APassengerCharacterBase::OnStopAtAllWay()
{
	UE_LOG(LogPassengerCharacter, Log, TEXT("Passenger %s entered all-way stop."), *GetNameSafe(this));

	if (APassengerAIController* AIController = Cast<APassengerAIController>(GetController()); IsValid(AIController))
	{
		AIController->OnStopAtAllWay(this);
	}
}

void APassengerCharacterBase::OnAllWayStopMyTurn()
{
	UE_LOG(LogPassengerCharacter, Log, TEXT("Passenger %s is my turn in all-way stop."), *GetNameSafe(this));

	if (APassengerAIController* AIController = Cast<APassengerAIController>(GetController()); IsValid(AIController))
	{
		AIController->OnAllWayStopMyTurn(this);
	}
}

void APassengerCharacterBase::OnExitAllWay()
{
	UE_LOG(LogPassengerCharacter, Log, TEXT("Passenger %s exited all-way stop."), *GetNameSafe(this));

	if (APassengerAIController* AIController = Cast<APassengerAIController>(GetController()); IsValid(AIController))
	{
		AIController->OnExitAllWay(this);
	}
}

FVector APassengerCharacterBase::GetDestinationLocation() const
{
	return DestinationLocation;
}

void APassengerCharacterBase::SmoothWalkSpeed()
{
	if (UCharacterMovementComponent* CharMove = GetCharacterMovement(); IsValid(CharMove))
	{
		const float Current = CharMove->MaxWalkSpeed;
		const float Target = DesiredSpeed;
		if (FMath::IsNearlyEqual(Current, Target, KINDA_SMALL_NUMBER))
		{
			GetWorldTimerManager().ClearTimer(WalkSpeedAdjustmentTimerHandle);
			bIsWalkSpeedAdjusting = false;
			return;
		}
		const float Diff = Target - Current;
		const float Step = Diff * FMath::Clamp(DefaultWalkSmoothingInterval * SpeedInterpRate, 0.f, 1.f);
		CharMove->MaxWalkSpeed = Current + Step;
	}
	else
	{
		UE_LOG(LogPassengerCharacter, Warning, TEXT("SmoothWalkSpeed: Invalid Character Movement Component!"));
		return;
	}
}

void APassengerCharacterBase::AdjustWalkSpeed(const float TargetSpeed)
{
	bIsWalkSpeedAdjusting = true;
	DesiredSpeed = TargetSpeed * WalkSpeed; // Scale the target speed by the current walk speed
	GetWorldTimerManager().SetTimer(WalkSpeedAdjustmentTimerHandle, this, &APassengerCharacterBase::SmoothWalkSpeed,
	                                DefaultWalkSmoothingInterval, true);
}


void APassengerCharacterBase::HandleGameOver(FLevelData LevelData)
{
	UE_LOG(LogPassengerCharacter, Warning, TEXT("Game Over! Level: %d, Did Win: %s, Score: %d"), LevelData.LevelNum,
	       LevelData.bDidWin ? TEXT("true") : TEXT("false"),
	       LevelData.Score);
	StopPassenger();

	// make character jump and play a death animation
	Jump();
}

void APassengerCharacterBase::OnInteract_Implementation(APlayerController* Caller)
{
	OnInteractPure(Caller);
}

void APassengerCharacterBase::StopPassenger()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	GetWorldTimerManager().ClearTimer(WalkSpeedUpdateTimerHandle);
	GetWorldTimerManager().ClearTimer(RevealTimerHandle);
	GetWorldTimerManager().ClearTimer(WalkSpeedUpdateTimerHandle);
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
		bIsMoving = false;
	}
}


void APassengerCharacterBase::OnInteractPure(APlayerController* Caller)
{
	if (!Caller) return;
	UE_LOG(LogPassengerCharacter, Warning, TEXT("Interacted with passenger: %s"),
	       *GetNameSafe(this));

	if (ANoNameGameModeBase* GM = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnCharacterInteracted.Broadcast(this);
	}

	if (InteractSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, InteractSoundCue, GetActorLocation());
	}
}

void APassengerCharacterBase::OnStartScan_Implementation(AScannerActor* ScannerActor)
{
	OnStartScanPure(ScannerActor);
}


void APassengerCharacterBase::OnStartScanPure(AScannerActor* ScannerActor)
{
	if (!IsValid(ScannerActor))
	{
		UE_LOG(LogPassengerCharacter, Warning, TEXT("ScannerActor is not valid!"));
		return;
	}

	ScannerActor->SetLightColor(ScannerColor);

	if (bRevealed)
	{
		// if already revealed, do nothing
		return;
	}

	ShowScanned();

	bWasScanned = true;

	if (ScanSoundCueStart)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ScanSoundCueStart, GetActorLocation());
	}
}

void APassengerCharacterBase::OnEndScan_Implementation(AScannerActor* ScannerActor)
{
	OnEndScanPure(ScannerActor);
}

void APassengerCharacterBase::OnEndScanPure(AScannerActor* ScannerActor)
{
	if (!IsValid(ScannerActor))
	{
		UE_LOG(LogPassengerCharacter, Warning, TEXT("ScannerActor is not valid!"));
		return;
	}

	if (const ANoNameGameModeBase* GM = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnCharacterScanned.Broadcast(this);
	}
	if (bRevealed) return;

	HideScanned();

	ScannerActor->SetLightColor(FLinearColor::Black);

	if (IsValid(ScanSoundCueEnd))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ScanSoundCueEnd, GetActorLocation());
	}
}
