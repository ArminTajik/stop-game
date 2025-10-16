


#include "UIComponents/Menus/GameOverUserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Gameplay/NoNameGameInstance.h"
#include "Gameplay/GameModes/NoNameGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UGameOverUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (WinText)
	{
		WinText->SetVisibility(ESlateVisibility::Hidden);
	}

	if (LoseText)
	{
		LoseText->SetVisibility(ESlateVisibility::Hidden);
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameOverUserWidget::RestartLevel);
	}

	if (QuitToMainMenuButton)
	{
		QuitToMainMenuButton->OnClicked.AddDynamic(this, &UGameOverUserWidget::QuitToMainMenu);
	}

	if (NextLevelButton)
	{
		NextLevelButton->OnClicked.AddDynamic(this, &UGameOverUserWidget::NextLevel);
		NextLevelButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGameOverUserWidget::GameOver(FLevelData LevelData)
{
	if (NextLevelButton)
	{
		NextLevelButton->SetVisibility(ESlateVisibility::Hidden);
	}

	if (LevelData.bDidWin)
	{
		if (WinText)
		{
			WinText->SetVisibility(ESlateVisibility::Visible);
		}
		if (LoseText)
		{
			LoseText->SetVisibility(ESlateVisibility::Hidden);
		}

		if (!LevelData.bLastLevel && NextLevelButton)
		{
			NextLevelButton->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (LoseText)
		{
			LoseText->SetVisibility(ESlateVisibility::Visible);
		}

		if (WinText)
		{
			WinText->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), LevelData.Score)));
		ScoreText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGameOverUserWidget::QuitToMainMenu()
{
	if (ANoNameGameModeBase* GameMode = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->QuitToMainMenu();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid game mode found to quit to main menu!"));
	}
}

void UGameOverUserWidget::RestartLevel()
{
	if (ANoNameGameModeBase* GameMode = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->RestartLevel();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid game mode found to quit to main menu!"));
	}
}

void UGameOverUserWidget::NextLevel()
{
	if (UNoNameGameInstance* GameInstance = Cast<UNoNameGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GameInstance->LoadNextLevel();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid game instance found to load next level!"));
	}
}
