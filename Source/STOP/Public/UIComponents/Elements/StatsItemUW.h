
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatsItemUW.generated.h"


class UHorizontalBox;

/**
 * 
 */
UCLASS()
class STOP_API UStatsItemUW : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	void SetText(const FText& NewText) const;

protected:

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* StatsText = nullptr;
	
};
