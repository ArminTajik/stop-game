

#include "Actors/TrapdoorActor.h"

#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Gameplay/NoNamePlayerController.h"
#include "Gameplay/GameModes/NoNameGameModeBase.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTrapdoor);

class ANoNamePlayerController;

// Sets default values
ATrapdoorActor::ATrapdoorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	TrapdoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScannerMesh"));
	TrapdoorMesh->SetupAttachment(RootComponent);
	TrapdoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TrapdoorMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TrapdoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	TapTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TapTrigger"));
	TapTrigger->SetupAttachment(RootComponent);
	TapTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TapTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	TapTrigger->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// Called when the game starts or when spawned
void ATrapdoorActor::BeginPlay()
{
	Super::BeginPlay();

	if (ANoNameGameModeBase* GameMode = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		// Bind the score and health change handlers
		GameMode->OnTrapdoorChangeActive.AddDynamic(this, &ATrapdoorActor::SetActiveTrap);
	}
	else
	{
		UE_LOG(LogTrapdoor, Warning, TEXT("No valid Game Mode found!"));
	}

	if (ANoNamePlayerController* PC = Cast<ANoNamePlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->AddTrapdoorButton(this);
	}
	else
	{
		UE_LOG(LogTrapdoor, Warning, TEXT("No valid Player Controller found!"));
	}

	// Delay the nav mesh rebuild slightly to ensure the trapdoor is fully registered in the world
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ATrapdoorActor::RebuildNavMesh,
		1.0f,
		false
	);
}

void ATrapdoorActor::RebuildNavMesh()
{
	// if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
	// {
	// 	NavSys->Build();
	// 	UE_LOG(LogTrapdoor, Warning, TEXT("NavMesh rebuilt from TrapdoorActor."));
	// }
}

void ATrapdoorActor::SetActiveTrap(bool bActive)
{
	UE_LOG(LogTrapdoor, Log, TEXT("Trapdoor %s"), bActive ? TEXT("Activated") : TEXT("Deactivated"));

	if (!IsValid(TrapdoorMesh)) return;

	TrapdoorMesh->SetVisibility(bActive);
	TrapdoorMesh->SetCollisionResponseToChannel(ECC_Pawn, bActive ? ECR_Block : ECR_Ignore);

	if (bActive && IsValid(OpenSoundCue))
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenSoundCue, GetActorLocation());
	}
	else if (!bActive && IsValid(CloseSoundCue))
	{
		UGameplayStatics::PlaySoundAtLocation(this, CloseSoundCue, GetActorLocation());
	}
}

void ATrapdoorActor::ToggleTrap()
{
	bIsActive = !bIsActive;
	SetActiveTrap(bIsActive);
}

void ATrapdoorActor::OnHoldStart_Implementation(APlayerController* Caller)
{
	OnHoldStartPure(Caller);
}
void ATrapdoorActor::OnHoldStartPure(APlayerController* Caller)
{
	SetActiveTrap(false);
}
void ATrapdoorActor::OnHoldEnd_Implementation(APlayerController* Caller)
{
	OnHoldEndPure(Caller);
}
void ATrapdoorActor::OnHoldEndPure(APlayerController* Caller)
{
	SetActiveTrap(true);
}

