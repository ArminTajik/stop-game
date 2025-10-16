#include "UIComponents/Helpers/UserWidgetStack.h"

DEFINE_LOG_CATEGORY(LogUI);

void UUserWidgetStack::PushUI(UUserWidget* NewUI)
{
	if (!ensureMsgf(NewUI, TEXT("PushUI: NewUI is null"))) return;

	if (!NewUI || UIStack.Num() > 0 && UIStack.Last() == NewUI)
	{
		UE_LOG(LogUI, Warning, TEXT("Attempted to push a null or duplicate UI onto the stack!"));
		return;
	}
	// If there's a current UI, remove it from viewport
	if (UIStack.Num() > 0)
	{
		if (UUserWidget* CurrentUI = UIStack.Last())
		{
			CurrentUI->RemoveFromParent();
		}
	}

	// Add the new UI to the stack and viewport
	if (NewUI)
	{
		UIStack.Add(NewUI);
		NewUI->AddToViewport();
	}
}

void UUserWidgetStack::PopUI()
{
	ensureAlwaysMsgf(IsInGameThread(), TEXT("PopUI must be called on game thread"));

	if (UIStack.Num() == 0) return;

	// Remove the current UI from viewport and stack
	if (UUserWidget* CurrentUI = UIStack.Pop())
	{
		CurrentUI->RemoveFromParent();
	}

	// If there's a previous UI, add it back to the viewport
	if (UIStack.Num() > 0)
	{
		if (UUserWidget* PreviousUI = UIStack.Last())
		{
			PreviousUI->AddToViewport();
		}
	}
}

UUserWidget* UUserWidgetStack::GetCurrentUI() const
{
	return UIStack.Num() > 0 ? UIStack.Last() : nullptr;
}

void UUserWidgetStack::ClearUIStack()
{
	for (UUserWidget* UI : UIStack)
	{
		if (IsValid(UI))
		{
			UI->RemoveFromParent();
		}
	}

	UIStack.Empty();
}
