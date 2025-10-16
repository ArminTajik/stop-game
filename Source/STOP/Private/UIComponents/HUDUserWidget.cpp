#include "UIComponents/HUDUserWidget.h"

#include "Actors/TrapdoorActor.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Gameplay/GameModes/NoNameGameModeBase.h"
#include "UIComponents/Elements/HealthBarUW.h"
#include "UIComponents/Elements/StatsItemUW.h"
#include "UIComponents/Elements/TrapdoorButtonUW.h"


void UHUDUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	

	if (PauseMenuButton)
	{
		PauseMenuButton->OnClicked.AddDynamic(this, &UHUDUserWidget::OnPauseMenuButtonClicked);
	}

	if (PowerUpButton)
	{
		PowerUpButton->OnClicked.AddDynamic(this, &UHUDUserWidget::OnSpecialMoveButtonClicked);
		PowerUpButton->SetIsEnabled(false); // Disable the button by default
	}
}


void UHUDUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateScore(0);
	UpdateHealth(0);
	ClearLog();
}

void UHUDUserWidget::UpdateScore(int32 NewScore) const
{
	if (ScoreStat)
	{
		ScoreStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewScore)));
	}
}

void UHUDUserWidget::UpdateHealth(int32 NewHealth)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealth(NewHealth);
	}

	AddToDebugText(FString::Printf(TEXT("Health updated: %d"), NewHealth));
}

void UHUDUserWidget::UpdateTime(const float NewTime) const
{
	const int32 Minutes = FMath::Clamp(FMath::FloorToInt(NewTime / 60.0f), 0, 99);
	const int32 Seconds = FMath::Clamp(FMath::FloorToInt(NewTime) % 60, 0, 59);
	const int32 Fraction = FMath::Clamp(static_cast<int32>((NewTime - FMath::FloorToInt(NewTime)) * 10.0f), 0, 9);

	if (TimeStat)
	{
		TimeStat->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d.%d"), Minutes, Seconds, Fraction)));
	}
}

void UHUDUserWidget::AddTrapdoorButton(ATrapdoorActor* TrapdoorActor)
{
	if (!ensure(TrapdoorButtonBox) || !ensure(TrapdoorActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("TrapdoorButtonBox or TrapdoorActor is not set!"));
		return;
	}

	UTrapdoorButtonUW* NewTrapdoorButton = CreateWidget<UTrapdoorButtonUW>(this, TrapdoorButtonClass, NAME_None);

	if (NewTrapdoorButton)
	{
		NewTrapdoorButton->OnTrapdoorButtonClicked.AddDynamic(this, &UHUDUserWidget::OnTrapdoorButtonPressed);
		NewTrapdoorButton->OnTrapdoorButtonReleased.AddDynamic(this, &UHUDUserWidget::OnTrapdoorButtonReleased);
	}
	UPanelSlot* NewSlot = TrapdoorButtonBox->AddChild(NewTrapdoorButton);
	if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(NewSlot))
	{
		HBoxSlot->SetPadding(FMargin(5.0f));
	}
}


void UHUDUserWidget::OnPauseMenuButtonClicked()
{
	if (ANoNameGameModeBase* GameMode = Cast<ANoNameGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->PauseGame();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid Player Controller found!"));
	}
}

void UHUDUserWidget::OnTrapdoorButtonPressed(ATrapdoorActor* TrapdoorActor)
{
	if (!TrapdoorActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("TrapdoorActor is not set!"));
		return;
	}
	TrapdoorActor->SetActiveTrap(false);
}

void UHUDUserWidget::OnTrapdoorButtonReleased(ATrapdoorActor* TrapdoorActor)
{
	if (!TrapdoorActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("TrapdoorActor is not set!"));
		return;
	}
	TrapdoorActor->SetActiveTrap(true);
}

void UHUDUserWidget::AddToDebugText(const FString& String)
{
	if (LogText)
	{
		const FString CurrentText = LogText->GetText().ToString();
		LogText->SetText(FText::FromString(CurrentText + "\n" + String));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DebugText is not set!"));
	}
}

void UHUDUserWidget::ClearLog()
{
	if (LogText)
	{
		LogText->SetText(FText::GetEmpty());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LogText is not set!"));
	}
}

void UHUDUserWidget::OnSpecialMoveButtonClicked()
{
	if (PowerUpButton)
	{
		PowerUpButton->SetIsEnabled(false); // Disable the button to prevent multiple clicks
	}

	if (PowerUpText)
	{
		PowerUpText->SetText(FText::FromString("!!!"));
	}

	if (CurrentSpecialMove == EPowerUp::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("No special move selected!"));
		return;
	}

	if (ANoNameGameModeBase* GameMode = Cast<ANoNameGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->OnPowerUpUsed.Broadcast(CurrentSpecialMove);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid Game Mode found!"));
	}
}

void UHUDUserWidget::SetPowerUp(EPowerUp MoveType)
{
	CurrentSpecialMove = MoveType;
	if (PowerUpButton)
	{
		PowerUpButton->SetIsEnabled(MoveType != EPowerUp::None);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpecialMoveButton is not set!"));
	}

	if (!PowerUpText)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpecialMoveText is not set!"));
		return;
	}
	switch (MoveType)
	{
	case EPowerUp::EliminateAllAliens:
		PowerUpText->SetText(FText::FromString("Clear"));
		break;
	case EPowerUp::RevealAllCharacters:
		PowerUpText->SetText(FText::FromString("See"));
		break;
	case EPowerUp::StopRandomSpawner:
		PowerUpText->SetText(FText::FromString("Stop"));
		break;
	default:
		PowerUpText->SetText(FText::FromString("!!!"));
		break;
	}
}
