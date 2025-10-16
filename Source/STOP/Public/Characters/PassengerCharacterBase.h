
#pragma once

#include "CoreMinimal.h"
#include "NoNameCharacter.h"
#include "Components/SplineComponent.h"
#include "DataAssets/CharacterDataAsset.h"
#include "Interfaces/InteractableInterface.h"
#include "Interfaces/ScannableInterface.h"
#include "PassengerCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPassengerCharacter, Log, All);

class AScannerActor;
class AScorePopupActor;
class UNpcManagerSubsystem;

USTRUCT(BlueprintType)
struct FCharacterType
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Type")
	FText TypeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer Tags;
};

UCLASS()
class STOP_API APassengerCharacterBase : public ACharacter, public IInteractableInterface, public IScannableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APassengerCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void PostInitializeComponents() override;

public:
	// PROPERTIES ==========================================================

	UPROPERTY(BlueprintReadOnly)
	int32 Priority = 0;

	UPROPERTY(BlueprintReadOnly)
	double SpawnTime;

	UPROPERTY(BlueprintReadOnly)
	int32 SpawnerID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="01|CharacterType")
	FCharacterType CharacterType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="01|CharacterType")
    TArray<FLinearColor> PossibleCharacterColors;

	UPROPERTY(VisibleAnywhere, Category = "01|Scan")
	USkeletalMeshComponent* ScanOverlayMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "01|Scan")
	FLinearColor ScannerColor = FLinearColor::Green;

	// Starting walking speed
	UPROPERTY(EditAnywhere, Category="01|Walk", meta=(ClampMin="0.01"))
	float DefaultInitialWalkSpeed = 200.0f;

	// Interval to update walking speed (in seconds)
	UPROPERTY(EditAnywhere, Category="01|Walk", meta=(ClampMin="0.01"))
	float DefaultWalkSpeedUpdateInterval = 5.0f;

	// Maximum walking speed
	UPROPERTY(EditAnywhere, Category="01|Walk", meta=(ClampMin="0.01"))
	float DefaultMaxWalkSpeed = 500.0f;

	// Decrement value for spawn rate
	UPROPERTY(EditAnywhere, Category="01|Walk", meta=(ClampMin="0.01"))
	float DefaultWalkSpeedIncrement = 0.1f;

	// Interval to update walking speed (in seconds)
	UPROPERTY(EditAnywhere, Category="01|Walk", meta=(ClampMin="0.01"))
	float DefaultWalkSmoothingInterval = 0.2f;

	UPROPERTY(EditAnywhere, Category = "01|Audio")
	USoundBase* ScanSoundCueStart;

	UPROPERTY(EditAnywhere, Category = "01|Audio")
	USoundBase* ScanSoundCueEnd;

	UPROPERTY(EditAnywhere, Category = "01|Audio")
	USoundBase* ExitSoundCue;

	UPROPERTY(EditAnywhere, Category = "01|Audio")
	USoundBase* EnterSoundCue;

	UPROPERTY(EditAnywhere, Category = "01|Audio")
	USoundBase* InteractSoundCue;


	// FUNCTIONS ==========================================================

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void StopPassenger();

	UFUNCTION()
	void StepStop();

	UFUNCTION()
	void StepWalk();

	UFUNCTION(BlueprintCallable)
	void InitializeMovement(const FVector& Destination);

	UFUNCTION(BlueprintCallable)
	void Exit();

	UFUNCTION(BlueprintCallable)
	void CoverCharacter();

	UFUNCTION(BlueprintCallable)
	void RevealCharacter();

	UFUNCTION(BlueprintCallable)
	void ShowScanned();

	UFUNCTION(BlueprintCallable)
	void HideScanned();

	UFUNCTION()
	void SetSpawnerID(const int32 ID);

	UFUNCTION()
	void OnStopAtAllWay();

	UFUNCTION()
	void OnAllWayStopMyTurn();

	UFUNCTION()
	void OnExitAllWay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float SpeedInterpRate = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Navigation Spacing")
	float CheckRadius = 215.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Navigation Spacing")
	float StopDistance = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Navigation Spacing")
	float ResumeDistance = 215.f;

	float DesiredSpeed = 0.f;


	UFUNCTION(BlueprintCallable)
	void MovePassenger(APassengerCharacterBase* PassengerCharacterBase, const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetDestinationLocation() const;

	UFUNCTION()
	void SmoothWalkSpeed();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void AdjustWalkSpeed(float TargetSpeed);

	UPROPERTY()
	APassengerCharacterBase* StoppedFor = nullptr;

protected:
	bool bWasScanned = false;
	bool bIsMoving = false;

	UFUNCTION()
	void UpdateWalkSpeed();

private:
	UPROPERTY()
	USplineComponent* SplineToFollow;

	UPROPERTY()
	FVector DestinationLocation;

	UPROPERTY()
	FTimerHandle LevelTimerHandle;

	UPROPERTY()
	FTimerHandle RevealTimerHandle;

	UPROPERTY()
	FTimerHandle WalkSpeedUpdateTimerHandle;

	UPROPERTY()
	FTimerHandle WalkSpeedAdjustmentTimerHandle;

	float DistanceAlongSpline = 0.0f;
	float WalkSpeed = 200.0f;
	float WalkSpeedUpdateInterval = 5.0f;
	float MaxWalkSpeed = 500.0f;
	float WalkSpeedIncrement = 0.1f;

	bool bRevealed;
	bool bIsWalkSpeedAdjusting;

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void HandleGameOver(FLevelData LevelData);

public:
	// INTERFACE IMPLEMENTATIONS =========================================

	// Interactable Interface ----------------------

	void OnInteract_Implementation(APlayerController* Caller) override; // Blueprint Native Event override


	virtual void OnInteractPure(APlayerController* Caller) override;

	// Scannable Interface ---------------------

	void OnStartScan_Implementation(AScannerActor* ScannerActor) override; // Blueprint Native Event override
	

	virtual void OnStartScanPure(AScannerActor* ScannerActor) override;

	void OnEndScan_Implementation(AScannerActor* ScannerActor) override; // Blueprint Native Event override

	virtual auto OnEndScanPure(AScannerActor* ScannerActor) -> void override;
};
