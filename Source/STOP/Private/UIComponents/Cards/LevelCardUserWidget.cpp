
#include "UIComponents/Cards/LevelCardUserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/AssetManager.h"


void ULevelCardUserWidget::InitFromInfo(ULevelDataAsset* InInfo, const float InBestTime)
{
	if (!InInfo) return;

	LevelInfo = InInfo;
	BestTimeSeconds = InBestTime;

	if (TitleText)
	{
		TitleText->SetText(InInfo->DisplayName);
	}
	
	if (ThumbnailImage && InInfo->Thumbnail.IsValid())
	{
		ThumbnailImage->SetBrushFromTexture(InInfo->Thumbnail.Get());
	}
	else if (ThumbnailImage && LevelInfo && LevelInfo->Thumbnail.ToSoftObjectPath().IsValid())
	{
		FStreamableManager& SM = UAssetManager::GetStreamableManager();
		SM.RequestAsyncLoad(LevelInfo->Thumbnail.ToSoftObjectPath(),
		                    FStreamableDelegate::CreateWeakLambda(this, [this]()
		                    {
			                    if (ThumbnailImage && LevelInfo && LevelInfo->Thumbnail.Get())
				                    ThumbnailImage->SetBrushFromTexture(LevelInfo->Thumbnail.Get());
		                    }));
	}

	if (BestTimeText)
	{
		if (InBestTime > 0.f)
		{
			const int32 Minutes = FMath::FloorToInt(InBestTime / 60.f);
			const int32 Seconds = FMath::FloorToInt(FMath::Fmod(InBestTime, 60.f));
			const int32 Milliseconds = FMath::FloorToInt((InBestTime - FMath::FloorToInt(InBestTime)) * 1000.f);
			const FString TimeString = FString::Printf(TEXT("%02d:%02d.%03d"), Minutes, Seconds, Milliseconds);
			BestTimeText->SetText(FText::FromString(TimeString));
		}
		else
		{
			BestTimeText->SetText(FText::FromString("00:00.000"));
		}
	}

	const bool bLocked = LevelInfo && !LevelInfo->bInitiallyUnlocked;
	ApplyLockedVisuals(bLocked);
}

void ULevelCardUserWidget::SetSelected(bool bIsSelected)
{
	Super::SetSelected(bIsSelected);
	if (Bg_Image)
	{
		Bg_Image->SetColorAndOpacity(bIsSelected ? FLinearColor::Green : FLinearColor::Black);
	}
}


void ULevelCardUserWidget::ApplyLockedVisuals(bool bLocked)
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
	
	if (BestTimeText)
	{
		BestTimeText->SetColorAndOpacity(bLocked ? FSlateColor(FLinearColor::Gray) : FSlateColor(FLinearColor::White));
	}
}
