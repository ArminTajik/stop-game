#include "Actors/AllWayStop.h"
#include "Characters/PassengerCharacterBase.h"
#include "Components/BoxComponent.h"

DEFINE_LOG_CATEGORY(LogAllWayStop);

AAllWayStop::AAllWayStop()
{
	PrimaryActorTick.bCanEverTick = false;

	QueueZone = CreateDefaultSubobject<UBoxComponent>(TEXT("QueueZone"));
	SetRootComponent(QueueZone);
	QueueZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	QueueZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	QueueZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	QueueZone->OnComponentBeginOverlap.AddDynamic(this, &AAllWayStop::OnQueueBegin);
	QueueZone->OnComponentEndOverlap.AddDynamic(this, &AAllWayStop::OnQueueEnd);
}

void AAllWayStop::Register(APassengerCharacterBase* P)
{
	if (!IsValid(P)) return;

	// If already queued, ignore
	for (auto& W : Queue)
	{
		if (W.Get() == P) return;
	}

	Queue.Add(P);
	TryAdvance();
}

void AAllWayStop::Unregister(APassengerCharacterBase* P)
{
	if (!IsValid(P)) return;

	// Remove from queue
	for (int32 i = Queue.Num() - 1; i >= 0; --i)
	{
		if (Queue[i].Get() == P) Queue.RemoveAt(i);
	}

	// If the current holder is leaving, clear and advance
	if (Current.Get() == P)
	{
		Current = nullptr;
		TryAdvance();
	}
}

void AAllWayStop::OnQueueBegin(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool,
                               const FHitResult&)
{
	if (auto* P = Cast<APassengerCharacterBase>(Other))
	{
		Register(P);

		// Everyone who is NOT current should slow/stop here
		if (Current.Get() != P)
		{
			P->OnStopAtAllWay();
		}
	}
}

void AAllWayStop::OnQueueEnd(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32)
{
	if (auto* P = Cast<APassengerCharacterBase>(Other))
	{
		// If someone backs out of the queue area, unqueue them
		Unregister(P);

		P->OnExitAllWay();
	}
}

void AAllWayStop::CleanupDeadEntries()
{
	for (int32 i = Queue.Num() - 1; i >= 0; --i)
		if (!Queue[i].IsValid()) Queue.RemoveAt(i);
}

void AAllWayStop::TryAdvance()
{
	CleanupDeadEntries();

	if (Current.IsValid()) return; // Someone already has the token
	if (Queue.Num() == 0) return;

	// Pop head until we find a valid pawn
	APassengerCharacterBase* Head = nullptr;
	while (Queue.Num() > 0 && !Head)
	{
		Head = Queue[0].Get();
		Queue.RemoveAt(0);
		if (!IsValid(Head)) Head = nullptr;
	}
	if (!Head) return;

	Current = Head;
	Grant(Head);
}

void AAllWayStop::Grant(APassengerCharacterBase* Passenger)
{
	if (!IsValid(Passenger)) return;
	
	Passenger->OnAllWayStopMyTurn();
}

void AAllWayStop::Revoke(APassengerCharacterBase* Passenger)
{
	if (!IsValid(Passenger)) return;
	Passenger->OnStopAtAllWay(); // Stop the passenger, they are no longer current
}
