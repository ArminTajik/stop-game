
#include "UIComponents/Menus/StoryUW.h"


#include "UIComponents/Menus/SelectLevelUserWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "DataAssets/StoryData.h"
#include "DataAssets/StoryDataList.h"
#include "DataPersistance/DataPersistenceManager.h"
#include "Engine/AssetManager.h"
#include "Gameplay/NoNameGameInstance.h"
#include "UIComponents/Cards/StoryCardUW.h"
#include "UIComponents/Elements/ButtonUW.h"

class UScrollBoxSlot;
DEFINE_LOG_CATEGORY(LogStoryMenu);

void UStoryUW::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BuyButton)
	{
		BuyButton->OnClicked.AddDynamic(this, &UStoryUW::HandleBuy);
		BuyButton->SetButtonText(FText::FromString("Unlock"));
	}

	if (ensure(StoryBox))
	{
		StoryBox->OnUserScrolled.AddDynamic(this, &UStoryUW::HandleScroll);
	}
}

void UStoryUW::NativeConstruct()
{
	Super::NativeConstruct();

	if (BuyButton)
	{
		BuyButton->SetIsEnabled(false);
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	BuildCards();
}

void UStoryUW::BuildCards()
{
	UE_LOG(LogStoryMenu, Log, TEXT("Building level selection grid..."));

	if (Cards.Num() > 0)
	{
		UE_LOG(LogStoryMenu, Warning, TEXT("Cards array is not empty! Clearing..."));
		Cards.Empty();
	}

	if (!StoryBox || !StoryCardClass)
	{
		UE_LOG(LogStoryMenu, Warning, TEXT("StoryBox or StoryCardClass is not set!"));
		return;
	}

	StoryBox->ClearChildren();

	if (!StoryList || StoryList->Stories.Num() == 0)
	{
		UE_LOG(LogStoryMenu, Warning, TEXT("StoryList is not set or contains no stories!"));
		return;
	}

	const UDataPersistenceManager* DPM = GetGameInstance()->GetSubsystem<UDataPersistenceManager>();
	if (!DPM)
	{
		UE_LOG(LogStoryMenu, Warning, TEXT("Failed to get DataPersistenceManager subsystem!"));
		return;
	}


	TArray<FPrimaryAssetId> UnlockedStories = DPM->GetUnlockedStoryIds();

	UStoryCardUW* CardToSelect = nullptr;
	bool bFoundSelected = false;
	for (int i = 0; i < StoryList->Stories.Num(); i++)
	{
		// get story data from asset manager
		UStoryData* Story = StoryList->Stories[i];

		if (!IsValid(Story))
		{
			UE_LOG(LogStoryMenu, Warning, TEXT("StoryList contains null entry at index %d"), i);
			continue;
		}

		const bool bIsUnlocked = Story->bInitiallyUnlocked || DPM->IsStoryUnlocked(Story->GetPrimaryAssetId());


		UStoryCardUW* Card = CreateWidget<UStoryCardUW>(this, StoryCardClass, NAME_None);
		Cards.Add(Card);
		Card->InitFromInfo(Story);

		Card->OnCardSelected.AddDynamic(this, &UStoryUW::HandleCardSelected);

		if (UScrollBoxSlot* NewSlot = Cast<UScrollBoxSlot>(StoryBox->AddChild(Card)))
		{
			NewSlot->SetPadding(FMargin(DefaultPadding));
		}

		if (!bFoundSelected && !bIsUnlocked)
		{
			CardToSelect = Card;
			bFoundSelected = true;
			break;
		}
	}

	if (Cards.Num() == 0)
	{
		UE_LOG(LogStoryMenu, Warning, TEXT("No valid stories to display after building cards!"));
		return;
	}

	if (!bFoundSelected)
	{
		CardToSelect = Cards.Last();
	}

	HandleCardSelected(CardToSelect);

	// make sure we scroll to it
	StoryBox->ScrollWidgetIntoView(CardToSelect, true,
	                               EDescendantScrollDestination::Center, 0.f);


	// If we failed to get metrics, try again next tick
	FTimerHandle SafeMetricTimer;
	GetWorld()->GetTimerManager().SetTimer(SafeMetricTimer, [this]()
	{
		if (IsValid(StoryBox))
		{
			StoryBox->ForceLayoutPrepass();
		}
		CacheUniformMetrics();
	}, 0.05f, false);
}

void UStoryUW::CacheUniformMetrics()
{
	check(Cards.Num() > 0);

	const float ViewW = StoryBox->GetCachedGeometry().GetLocalSize().X;

	// First card
	if (UScrollBoxSlot* FirstSlot = Cast<UScrollBoxSlot>(Cards[0]->Slot))
	{
		const float W = Cards[0]->GetDesiredSize().X;
		const float LeftPad = (ViewW * 0.5f) - (W * 0.5f);
		FirstSlot->SetPadding(FMargin(LeftPad, DefaultPadding, DefaultPadding, DefaultPadding));
	}

	// Last card
	if (UScrollBoxSlot* LastSlot = Cast<UScrollBoxSlot>(Cards.Last()->Slot))
	{
		const float W = Cards.Last()->GetDesiredSize().X;
		const float RightPad = (ViewW * 0.5f) - (W * 0.5f);
		LastSlot->SetPadding(FMargin(DefaultPadding, DefaultPadding, RightPad, DefaultPadding));
	}


	ItemW = Cards[0]->GetDesiredSize().X;

	// read right padding from the first slot
	Gap = 0.f;
	float LeadPad = 0.f;
	if (UScrollBoxSlot* Slot0 = Cast<UScrollBoxSlot>(Cards[0]->Slot))
	{
		FMargin CurrentPadding = Slot0->GetPadding();
		LeadPad = CurrentPadding.Left;
		Gap = DefaultPadding;
	}

	Pitch = ItemW + Gap;

	FirstCenter = LeadPad + (ItemW * 0.5f);
}

void UStoryUW::HandleCardSelected(UCardUW* Card)
{
	if (!IsValid(Card))
	{
		UE_LOG(LogStoryMenu, Log, TEXT("Selected card is null"));
		return;
	}


	const TObjectPtr<UStoryCardUW> StoryCard = Cast<UStoryCardUW>(Card);
	if (!IsValid(StoryCard))
	{
		UE_LOG(LogStoryMenu, Log, TEXT("Selected card is not a StoryCardUW"));
		return;
	}

	if (IsValid(SelectedCard) && SelectedCard != StoryCard)
	{
		SelectedCard->SetSelected(false);
	}

	SelectedCard = StoryCard;

	if (IsValid(Card))
	{
		Card->SetSelected(true);
		if (BuyButton)
		{
			// score check
			if (const UDataPersistenceManager* DPM = GetGameInstance()->GetSubsystem<UDataPersistenceManager>())
			{
				const int32 PlayerScore = DPM->GetCurrentScore();
				const int32 StoryPrice = StoryCard->GetStoryInfo() ? StoryCard->GetStoryInfo()->Price : 0;
				const bool bCanAfford = PlayerScore >= StoryPrice;
				const bool bIsUnlocked = DPM->IsStoryUnlocked(StoryCard->GetStoryInfo()->GetPrimaryAssetId()) ||
					StoryCard->GetStoryInfo()->bInitiallyUnlocked;
				BuyButton->SetIsEnabled(bCanAfford && !bIsUnlocked);
			}
		}
	}
}

void UStoryUW::HandleBuy()
{
	if (!IsValid(SelectedCard))
	{
		UE_LOG(LogStoryMenu, Warning, TEXT("No level selected!"));
		return;
	}

	if (UNoNameGameInstance* GI = GetGameInstance<UNoNameGameInstance>())
	{
		GI->PurchaseStory(SelectedCard->GetStoryInfo());
	}
	else
	{
		UE_LOG(LogLevelSelectionMenu, Warning, TEXT("Failed to get Game Instance!"));
	}

	BuildCards();
}


void UStoryUW::SnapToNearestCard(UCardUW* Card)
{
	bIsSnapping = true;

	UWidget* Target = Card;
	StoryBox->ScrollWidgetIntoView(Target, true,
	                               EDescendantScrollDestination::Center, 0.f);

	// let the animation settle
	GetWorld()->GetTimerManager().ClearTimer(SnapAnimGuardTimer);
	GetWorld()->GetTimerManager().SetTimer(SnapAnimGuardTimer, [this]()
	{
		bIsSnapping = false;
	}, 0.20f, false);
}

int32 UStoryUW::IndexFromScroll() const
{
	const float ViewW = StoryBox->GetCachedGeometry().GetLocalSize().X;
	const float ViewCenter = StoryBox->GetScrollOffset() + (ViewW * 0.5f);

	// How many pitches from the first center are we?
	const float FIdx = (ViewCenter - FirstCenter) / Pitch;
	return FMath::Clamp(FMath::RoundToInt(FIdx), 0, Cards.Num() - 1);
}

void UStoryUW::HandleScroll(float CurrentOffset)
{
	if (!StoryBox || Cards.Num() == 0 || Pitch <= KINDA_SMALL_NUMBER) return;

	const int32 Best = IndexFromScroll();
	UCardUW* Target = Cards[Best];

	HandleCardSelected(Target);

	if (bIsSnapping) return;
	GetWorld()->GetTimerManager().ClearTimer(SnapIdleTimer);
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UStoryUW::SnapToNearestCard, Target);
	GetWorld()->GetTimerManager().SetTimer(SnapIdleTimer, TimerDelegate, SnapIdleDelay, false);
}
