
#include "UIComponents/Menus/MenuScreenUW.h"

#include "Components/Button.h"
#include "Gameplay/MainMenuPlayerController.h"
#include "UIComponents/Elements/ButtonUW.h"


void UMenuScreenUW::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UMenuScreenUW::HandleBack);
		BackButton->SetButtonText(FText::FromString("Back"));
	}
}

void UMenuScreenUW::HandleBack()
{
	if (AMainMenuPlayerController* PC = Cast<AMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->BackToMainMenu();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get MainMenuPlayerController!"));
	}
}
