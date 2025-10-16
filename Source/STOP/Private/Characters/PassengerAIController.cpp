

#include "Characters/PassengerAIController.h"
#include "Characters/PassengerCharacterBase.h"
#include "Subsystems/NpcManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogPassengerAIController);

APassengerAIController::APassengerAIController()
{
}

void APassengerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogPassengerAIController, Log, TEXT("Possessing pawn: %s, spawn time: %.2f"),
	       *GetNameSafe(InPawn), FPlatformTime::Seconds());

	// Ensure movement is delayed until controller is ready
	GetWorldTimerManager().SetTimer(StartMoveTimerHandle, this, &APassengerAIController::StartMove,
	                                0.04f,
	                                false);
}

// Function to start moving the passenger to the destination
void APassengerAIController::StartMove()
{
	GetWorldTimerManager().ClearTimer(StartMoveTimerHandle);
	if (const APassengerCharacterBase* Passenger = Cast<APassengerCharacterBase>(GetPawn()))
	{
		SetDestination(Passenger->GetDestinationLocation());
		MoveToLocation(Destination);
	}

	GetWorld()->GetTimerManager().SetTimer(
		SpacingCheckTimerHandle,
		this,
		&APassengerAIController::CheckSpacingAndAct,
		0.1f,
		true
	);
}

void APassengerAIController::StopMove()
{
	const APawn* ControlledPawn = GetPawn();
	GetWorldTimerManager().ClearTimer(SpacingCheckTimerHandle);
	if (IsValid(ControlledPawn))
	{
		UE_LOG(LogPassengerAIController, Log, TEXT("Stopping movement for pawn: %s"), *GetNameSafe(ControlledPawn));
		StopMovement();
		bIsStopped = true;
	}
	else
	{
		UE_LOG(LogPassengerAIController, Warning, TEXT("No pawn to stop movement for!"));
	}
}

void APassengerAIController::OnStopAtAllWay(APassengerCharacterBase* ThisPassenger)
{
	GetWorldTimerManager().ClearTimer(SpacingCheckTimerHandle);
	bIsStopped = true;
	if (IsValid(ThisPassenger))
	{
		UE_LOG(LogPassengerAIController, Log, TEXT("Stopping movement for pawn: %s at all-way stop!"),
		       *GetNameSafe(ThisPassenger));
		ThisPassenger->AdjustWalkSpeed(0.f);
	}
	else
	{
		UE_LOG(LogPassengerAIController, Warning, TEXT("No valid passenger to stop at all-way stop!"));
	}
}

void APassengerAIController::OnAllWayStopMyTurn(APassengerCharacterBase* ThisPassenger)
{
	bIsStopped = false;
	if (IsValid(ThisPassenger))
	{
		UE_LOG(LogPassengerAIController, Log, TEXT("Resuming movement for pawn: %s at all-way stop!"),
		       *GetNameSafe(ThisPassenger));
		ThisPassenger->AdjustWalkSpeed(1.f);
	}
	else
	{
		UE_LOG(LogPassengerAIController, Warning, TEXT("No valid passenger to resume at all-way stop!"));
	}
}

void APassengerAIController::OnExitAllWay(const APassengerCharacterBase* ThisPassenger)
{
	if (!IsValid(ThisPassenger)) return;
	UE_LOG(LogPassengerAIController, Log, TEXT("Exiting all-way stop for pawn: %s"), *GetNameSafe(ThisPassenger));
	bIsStopped = false;
	GetWorld()->GetTimerManager().SetTimer(
		SpacingCheckTimerHandle,
		this,
		&APassengerAIController::CheckSpacingAndAct,
		0.1f,
		true
	);
}

void APassengerAIController::OnUnPossess()
{
	UE_LOG(LogPassengerAIController, Log, TEXT("Unpossessing pawn: %s"), *GetNameSafe(GetPawn()));
	GetWorld()->GetTimerManager().ClearTimer(SpacingCheckTimerHandle);
	Super::OnUnPossess();
}

void APassengerAIController::SetDestination(const FVector& InDestination)
{
	UE_LOG(LogPassengerAIController, Verbose, TEXT("Setting destination for pawn: %s to %s"),
	       *GetNameSafe(GetPawn()), *InDestination.ToString());
	Destination = InDestination;
}

void APassengerAIController::YieldForOther(APassengerCharacterBase* Other, APassengerCharacterBase* ThisPassenger)
{
	DrawDebugLine(GetWorld(), ThisPassenger->GetActorLocation(), Other->GetActorLocation(),
	              FColor::White, false, 0.1f, 0, (ThisPassenger->Priority * 0.2f) + 1.f);

	if (Other == ThisPassenger->StoppedFor)
	{
		UE_LOG(LogPassengerAIController, Verbose,
		       TEXT("Yielding for the same passenger: %s, ignoring."), *GetNameSafe(Other));
		return;
	}

	ThisPassenger->AdjustWalkSpeed(0.f);
	bIsStopped = true;
	ThisPassenger->StoppedFor = Other;
	ThisPassenger->Priority++;

	UE_LOG(LogPassengerAIController, Verbose,
	       TEXT(
		       "Stopping movement for pawn: %s due to nearby pawn: %s, spawn tine %.2f vs %.2f"
	       ),
	       *GetNameSafe(ThisPassenger), *GetNameSafe(Other), ThisPassenger->SpawnTime,
	       Other->SpawnTime);
}

void APassengerAIController::MoveAhead(APassengerCharacterBase* ThisPassenger)
{
	UE_LOG(LogPassengerAIController, Verbose, TEXT("Resuming movement for pawn: %s"),
	       *GetNameSafe(ThisPassenger));

	bIsStopped = false;
	ThisPassenger->StoppedFor = nullptr;
	ThisPassenger->AdjustWalkSpeed(1.f);
}

void APassengerAIController::CheckSpacingAndAct()
{
	UE_LOG(LogPassengerAIController, VeryVerbose, TEXT("Checking spacing for pawn: %s"), *GetNameSafe(GetPawn()));

	APassengerCharacterBase* ThisPassenger = Cast<APassengerCharacterBase>(GetPawn());
	if (!IsValid(ThisPassenger)) return;

	const float CheckRadius = ThisPassenger->CheckRadius;
	const float StopDistance = ThisPassenger->StopDistance;
	const float ResumeDistance = ThisPassenger->ResumeDistance;

	if (const UGameInstance* GI = GetGameInstance())
	{
		if (UNpcManagerSubsystem* Manager = GI->GetSubsystem<UNpcManagerSubsystem>())
		{
			TArray<APassengerCharacterBase*> Nearby = Manager->GetNearbyNPCs(
				ThisPassenger->GetActorLocation(), CheckRadius);

			UE_LOG(LogPassengerAIController, VeryVerbose, TEXT("Found %d nearby pawns for: %s"),
			       Nearby.Num(), *GetNameSafe(ThisPassenger));

			bool bShouldStayStopped = false;

			for (APassengerCharacterBase* Other : Nearby)
			{
				if (!IsValid(Other) || Other == ThisPassenger || Other->StoppedFor == ThisPassenger) continue;

				FVector ToOther = Other->GetActorLocation() - ThisPassenger->GetActorLocation();

				const bool IsInFront = FVector::DotProduct(ThisPassenger->GetActorForwardVector(),
				                                           ToOther.GetSafeNormal()) > 0.5f;

				if (const float Distance = ToOther.Size(); IsInFront && Distance < StopDistance)
				{
					// If the other passenger is already stopped for this passenger, ignore
					if (Other->StoppedFor == ThisPassenger)
					{
						UE_LOG(LogPassengerAIController, Verbose,
						       TEXT("Other passenger %s is already stopped for this passenger: %s"),
						       *GetNameSafe(Other), *GetNameSafe(ThisPassenger));
						continue;
					}
					
					// If the other passenger has lower or equal priority, yield for the other passenger
					if (ThisPassenger->Priority <= Other->Priority)
					{
						YieldForOther(Other, ThisPassenger);
						return;
					}

					// If the other passenger has higher priority, check their spawn time and distance to destination
					const float DistanceToDestination = (ThisPassenger->GetActorLocation() - Destination).Size();
					const float OtherDistanceToDestination = (Other->GetActorLocation() - Other->
						GetDestinationLocation()).Size();
					

					if (DistanceToDestination > OtherDistanceToDestination)
					{
						// If this passenger has higher spawn time or is further from destination, yield for the other
						YieldForOther(Other, ThisPassenger);
						return;
					}

					// If both passengers have the same spawn time or distance to destination, use unique ID as tie-breaker
					if (FMath::IsNearlyEqual(DistanceToDestination, OtherDistanceToDestination, KINDA_SMALL_NUMBER))
					// tie-breaker
					{
						const int32 ThisID = ThisPassenger->GetUniqueID();
						const int32 OtherID = Other->GetUniqueID();

						if (ThisID > OtherID)
						{
							YieldForOther(Other, ThisPassenger);
							return;
						}
					}
				}

				// If the other passenger is in front but further than the resume distance, we can resume movement
				else if (IsInFront && Distance < ResumeDistance)
				{
					bShouldStayStopped = true;
				}
			}

			// If we are stopped and no other passenger is in front or within resume distance, move ahead
			if (bIsStopped && !bShouldStayStopped)
			{
				MoveAhead(ThisPassenger);
			}
		}
	}
}
