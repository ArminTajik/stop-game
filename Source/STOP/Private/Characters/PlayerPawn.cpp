#include "Characters/PlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerPawn::UpdateCameraOrthographicSize(float DeltaSize)
{
	if (CameraComponent)
	{
		const float NewSize = CameraComponent->OrthoWidth + DeltaSize * 100.f; // Adjust the zoom sensitivity as needed
		CameraComponent->SetOrthoWidth(FMath::Clamp(NewSize, 1000.f, 3000.f)); // Clamp the size to a reasonable range
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraComponent is not valid!"));
	}
}

void APlayerPawn::UpdateCameraRotation(float X)
{
	if (CameraBoom)
	{
		FRotator NewRotation = CameraBoom->GetRelativeRotation();
		NewRotation.Yaw += X; // Adjust the rotation sensitivity as needed
		CameraBoom->SetRelativeRotation(NewRotation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraBoom is not valid!"));
	}
}

