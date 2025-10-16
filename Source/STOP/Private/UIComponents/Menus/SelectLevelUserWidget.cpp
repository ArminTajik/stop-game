
#include "UIComponents/Menus/SelectLevelUserWidget.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "DataAssets/LevelListDataAsset.h"
#include "Gameplay/MainMenuPlayerController.h"
#include "Gameplay/NoNameGameInstance.h"
#include "UIComponents/Cards/LevelCardUserWidget.h"
#include "UIComponents/Elements/ButtonUW.h"

class AMainMenuPlayerController;
DEFINE_LOG_CATEGORY(LogLevelSelectionMenu);

void USelectLevelUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &USelectLevelUserWidget::HandleStart);
		StartButton->SetButtonText(FText::FromString("Start"));
	}
}

void USelectLevelUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->SetIsEnabled(false);
	}

	BuildCards();
}

void USelectLevelUserWidget::BuildCards()
{
	UE_LOG(LogLevelSelectionMenu, Log, TEXT("Building level selection grid..."));
	if (!Grid_Levels || !LevelCardClass) return;

	Grid_Levels->ClearChildren();
	int32 Row = 0, Col = 0;

	for (ULevelDataAsset* Info : LevelList->Levels)
	{
		UE_LOG(LogLevelSelectionMenu, Log, TEXT("Adding level card for: %s"), *Info->DisplayName.ToString());
		const float BestTime = 0.f; // TODO: get from save data

		ULevelCardUserWidget* Card = CreateWidget<ULevelCardUserWidget>(GetOwningPlayer(), LevelCardClass, NAME_None);
		Card->InitFromInfo(Info, BestTime);

		// Bind selection event
		Card->OnCardSelected.AddDynamic(this, &USelectLevelUserWidget::HandleCardSelected);

		// Set the size of the slot
		if (UUniformGridSlot* NewSlot = Grid_Levels->AddChildToUniformGrid(Card, Row, Col))
		{
			NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			NewSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}
		Col++;
		if (constexpr int32 Columns = 2; Col >= Columns)
		{
			Col = 0;
			Row++;
		}
	}
}

void USelectLevelUserWidget::HandleCardSelected(UCardUW* Card)
{
	if (!IsValid(Card))
	{
		UE_LOG(LogLevelSelectionMenu, Log, TEXT("Selected card is null"));
		return;
	}

	const TObjectPtr<ULevelCardUserWidget> LevelCard = Cast<ULevelCardUserWidget>(Card); 
	if (!IsValid(LevelCard))
	{
		UE_LOG(LogLevelSelectionMenu, Log, TEXT("Selected card is not a LevelCardUserWidget"));
		return;
	}

	if (IsValid(SelectedCard) && SelectedCard != LevelCard)
	{
		SelectedCard->SetSelected(false);
	}
	
	SelectedCard = LevelCard;

	if (IsValid(Card))
	{
		Card->SetSelected(true);
		if (StartButton)
		{
			StartButton->SetIsEnabled(true);
		}
	}
}


void USelectLevelUserWidget::HandleStart()
{
	if (!IsValid(SelectedCard))
	{
		UE_LOG(LogLevelSelectionMenu, Warning, TEXT("No level selected!"));
		return;
	}

	
	
	ULevelDataAsset* SelectedLevel = SelectedCard->LevelInfo;
	if (!SelectedLevel)
	{
		UE_LOG(LogLevelSelectionMenu, Warning, TEXT("Selected level info is null!"));
		return;
	}
	if (AMainMenuPlayerController* PC = GetOwningPlayer<AMainMenuPlayerController>())
	{
		PC->HandleStart(SelectedLevel);
	}
	else
	{
		UE_LOG(LogLevelSelectionMenu, Warning, TEXT("Failed to get owning player controller!"));
		return;
	}
	
}
