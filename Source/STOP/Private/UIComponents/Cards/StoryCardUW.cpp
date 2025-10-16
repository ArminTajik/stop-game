
#include "UIComponents/Cards/StoryCardUW.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DataAssets/StoryData.h"
#include "Engine/AssetManager.h"


void UStoryCardUW::InitFromInfo(UStoryData* InitInfo)
{
	if (IsValid(InitInfo))
	{
		UE_LOG(LogTemp, Log, TEXT("Initializing StoryCardUW with info: %s"), *InitInfo->Title.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitFromInfo called with null InitInfo!"));
		return;
	}

	Info = InitInfo;

	if (TitleText)
	{
		TitleText->SetText(InitInfo->Title);
	}
	if (PriceText)
	{
		PriceText->SetText(FText::AsCurrency(InitInfo->Price));
	}

	
	if (ThumbnailImage && InitInfo->Image.IsValid()) // Direct set if already loaded
	{
		ThumbnailImage->SetBrushFromTexture(InitInfo->Image.Get());
	}
	else if (ThumbnailImage && Info && Info->Image.ToSoftObjectPath().IsValid()) // Lazy load
	{
		FStreamableManager& SM = UAssetManager::GetStreamableManager();
		SM.RequestAsyncLoad(Info->Image.ToSoftObjectPath(),
		                    FStreamableDelegate::CreateWeakLambda(this, [this]()
		                    {
			                    if (ThumbnailImage && Info && Info->Image.Get())
				                    ThumbnailImage->SetBrushFromTexture(Info->Image.Get());
		                    }));
	}
	

	const bool bLocked = Info && !Info->bInitiallyUnlocked;
	ApplyLockedVisuals(bLocked);
}

void UStoryCardUW::SetSelected(bool bIsSelected)
{
	if (Bg_Image)
	{
		Bg_Image->SetColorAndOpacity(bIsSelected ? FLinearColor::Green : FLinearColor::Black);
	}
}



void UStoryCardUW::ApplyLockedVisuals(bool bLocked)
{
	if (RootButton)
	{
		RootButton->SetIsEnabled(!bLocked);
	}
	if (ThumbnailImage)
	{
		ThumbnailImage->SetOpacity(bLocked ? 0.5f : 1.f);
	}
	if (TitleText)
	{
		TitleText->SetColorAndOpacity(bLocked ? FSlateColor(FLinearColor::Gray) : FSlateColor(FLinearColor::White));
	}
	if (PriceText)
	{
		PriceText->SetColorAndOpacity(
			bLocked ? FSlateColor(FLinearColor::Gray) : FSlateColor(FLinearColor::White));
	}
}
