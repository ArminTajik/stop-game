

#include "UIComponents/Menus/MainMenuUserWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Gameplay/MainMenuPlayerController.h"
#include "UIComponents/Elements/ButtonUW.h"

DEFINE_LOG_CATEGORY(LogMainMenuUserWidget);


void UMainMenuUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BindWidgets();
}


void UMainMenuUserWidget::BindWidgets()
{
	if (SelectLevelButton)
	{
		SelectLevelButton->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnSelectLevelButtonClicked);
		SelectLevelButton->SetButtonText(FText::FromString("Start"));
		SelectLevelButton->SetKeyboardFocus();
	}

	if (OptionsButton)
	{
		OptionsButton->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnOptionsButtonClicked);
 		OptionsButton->SetButtonText(FText::FromString("Options"));
	}

	if (CreditsButton)
	{
		CreditsButton->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnCreditsButtonClicked);
		CreditsButton->SetButtonText(FText::FromString("Credits"));
	}

	if (StoryButton)
	{
		StoryButton->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnStoryButtonClicked);
		StoryButton->SetButtonText(FText::FromString("Story"));
	}
}

void UMainMenuUserWidget::OnSelectLevelButtonClicked()
{
	if (AMainMenuPlayerController* PC = Cast<AMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->ShowLevelSelectionMenu();
	}
	else
	{
		UE_LOG(LogMainMenuUserWidget, Warning, TEXT("Failed to get MainMenuPlayerController!"));
	}
}

void UMainMenuUserWidget::OnControlsButtonClicked()
{
	if (AMainMenuPlayerController* PC = Cast<AMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->ShowControlsMenu();
	}
	else
	{
		UE_LOG(LogMainMenuUserWidget, Warning, TEXT("Failed to get MainMenuPlayerController!"));
	}
}

void UMainMenuUserWidget::OnOptionsButtonClicked()
{
	if (AMainMenuPlayerController* PC = Cast<AMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->ShowSettingsMenu();
	}
	else
	{
		UE_LOG(LogMainMenuUserWidget, Warning, TEXT("Failed to get MainMenuPlayerController!"));
	}
}

void UMainMenuUserWidget::OnCreditsButtonClicked()
{
	if (AMainMenuPlayerController* PC = Cast<AMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->ShowCreditsMenu();
	}
	else
	{
		UE_LOG(LogMainMenuUserWidget, Warning, TEXT("Failed to get MainMenuPlayerController!"));
	}
}

void UMainMenuUserWidget::OnStoryButtonClicked()
{
	if (AMainMenuPlayerController* PC = Cast<AMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->ShowStoryMenu();
	}
	else
	{
		UE_LOG(LogMainMenuUserWidget, Warning, TEXT("Failed to get MainMenuPlayerController!"));
	}
}
