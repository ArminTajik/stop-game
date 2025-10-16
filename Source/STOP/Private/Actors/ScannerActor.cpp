

#include "Actors/ScannerActor.h"

#include "Components/BoxComponent.h"
#include "Components/SpotLightComponent.h"
#include "Interfaces/ScannableInterface.h"

class APassengerCharacterBase;
// Sets default values
AScannerActor::AScannerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ScanTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ScanTrigger"));
	ScanTrigger->SetupAttachment(RootComponent);
	ScanTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ScanTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	ScanTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	ScanTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ScannerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScannerMesh"));
	ScannerMesh->SetupAttachment(RootComponent);
	ScannerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScannerMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ScannerMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	ScannerLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("ScannerLight"));
	ScannerLight->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AScannerActor::BeginPlay()
{
	Super::BeginPlay();

	ScanTrigger->OnComponentBeginOverlap.AddDynamic(this, &AScannerActor::OnStartScanOverlap);
	ScanTrigger->OnComponentEndOverlap.AddDynamic(this, &AScannerActor::OnEndScanOverlap);
}

void AScannerActor::OnStartScanOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								   const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UScannableInterface::StaticClass()))
	{
		IScannableInterface::Execute_OnStartScan(OtherActor, this);
	}
}

void AScannerActor::OnEndScanOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UScannableInterface::StaticClass()))
	{
		IScannableInterface::Execute_OnEndScan(OtherActor, this);
	}
}

void AScannerActor::SetLightColor(const FLinearColor LinearColor) const
{
	if (ScannerLight)
	{
		ScannerLight->SetLightColor(LinearColor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ScannerLight is not initialized!"), *GetClass()->GetName());
	}
}

