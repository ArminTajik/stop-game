
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeUW.generated.h"


/**
 * 
 */
UCLASS()
class STOP_API UFadeUW : public UUserWidget
{
	GENERATED_BODY()

public:
	void FadeInOut() { if (FadeInOutAnim) PlayAnimation(FadeInOutAnim); }

protected:
	// animation
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInOutAnim;
};
