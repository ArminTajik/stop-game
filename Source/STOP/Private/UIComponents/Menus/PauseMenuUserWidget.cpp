

#include "UIComponents/Menus/PauseMenuUserWidget.h"
#include "Gameplay/GameModes/NoNameGameModeBase.h"
#include "UIComponents/Elements/ButtonUW.h"

void UPauseMenuUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();


	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuUserWidget::Resume);
		ResumeButton->SetButtonText(FText::FromString("Resume"));
		ResumeButton->SetKeyboardFocus();
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ResumeButton is not set!"), *GetClass()->GetName());
	}

	if (QuitToMainMenuButton)
	{
		QuitToMainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuUserWidget::QuitToMainMenu);
		QuitToMainMenuButton->SetButtonText(FText::FromString("Main Menu"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] QuitToMainMenuButton is not set!"), *GetClass()->GetName());
	}
}

void UPauseMenuUserWidget::QuitToMainMenu()
{
	if (ANoNameGameModeBase* GameMode = Cast<ANoNameGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->QuitToMainMenu();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid Game Mode found!"));
	}
}

void UPauseMenuUserWidget::Resume()
{
	if (ANoNameGameModeBase* GameMode = Cast<ANoNameGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->ResumeGame();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid Game Mode found!"));
	}
}
