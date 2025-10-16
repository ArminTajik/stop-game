#include "Actors/UI/ScorePopUpActor.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"

AScorePopupActor::AScorePopupActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	RootComponent = WidgetComponent;
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawSize(FVector2D(200, 100));
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetComponent->SetTwoSided(true);

	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

}

void AScorePopupActor::Init(FText Text, float Duration, FColor TextColor)
{
	UE_LOG(LogTemp, Log, TEXT("[%s] Initializing score popup with text: %s, duration: %f, color: %s"),
	       *GetClass()->GetName(), *Text.ToString(), Duration, *TextColor.ToString());
	if (WidgetClass)
	{
		WidgetComponent->SetWidgetClass(WidgetClass);
		WidgetComponent->InitWidget(); // Make sure it actually creates the widget
	}
	if (UUserWidget* Widget = WidgetComponent->GetUserWidgetObject())
	{
		if (UTextBlock* ScoreText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("ScoreText"))))
		{
			UE_LOG(LogTemp, Log, TEXT("[%s] Found ScoreText widget."), *GetClass()->GetName());
			ScoreText->SetText(Text);
			ScoreText->SetColorAndOpacity(FSlateColor(TextColor));
		}
	}

	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AScorePopupActor::DestroyPopup, Duration, false);
}

void AScorePopupActor::DestroyPopup()
{
	GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
	Destroy();
}
