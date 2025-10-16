#include "Actors/LavaActor.h"
#include "Characters/PassengerCharacterBase.h"
#include "Components/BoxComponent.h"
#include "Gameplay/GameModes/NoNameGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALavaActor::ALavaActor()
{
	PrimaryActorTick.bCanEverTick = false;


	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	LavaTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ScanTrigger"));
	LavaTrigger->SetupAttachment(RootComponent);
	LavaTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LavaTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	LavaTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	LavaTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void ALavaActor::BeginPlay()
{
	Super::BeginPlay();
	LavaTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALavaActor::OnLavaOverlap);
}

void ALavaActor::OnLavaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult)
{
	if (APassengerCharacterBase* Passenger = Cast<APassengerCharacterBase>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Passenger %s entered lava!"), *GetClass()->GetName(),
		       *GetNameSafe(Passenger));
		if (ANoNameGameModeBase* GM = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			GM->OnCharacterDestroyed.Broadcast(Passenger);
		}
	}
}
