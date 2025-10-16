

#include "Actors/ExitDoorActor.h"
#include "Characters/PassengerCharacterBase.h"
#include "Components/BoxComponent.h"

// Sets default values
AExitDoorActor::AExitDoorActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ExitTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ScanTrigger"));
	ExitTrigger->SetupAttachment(RootComponent);
	ExitTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExitTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	ExitTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExitTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	VisualMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMarker"));
	VisualMarker->SetupAttachment(ExitTrigger);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube")); // Load a basic cube mesh
	if (CubeMesh.Succeeded())
	{
		VisualMarker->SetStaticMesh(CubeMesh.Object);
		VisualMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		VisualMarker->SetGenerateOverlapEvents(false);
		VisualMarker->SetVisibility(true);
		VisualMarker->SetHiddenInGame(true);
	}
}

// Called when the game starts or when spawned
void AExitDoorActor::BeginPlay()
{
	Super::BeginPlay();

	ExitTrigger->OnComponentBeginOverlap.AddDynamic(this, &AExitDoorActor::OnExitOverlap);
}


void AExitDoorActor::OnExitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (APassengerCharacterBase* Passenger = Cast<APassengerCharacterBase>(OtherActor))
	{
		Passenger->Exit();
	}
}
