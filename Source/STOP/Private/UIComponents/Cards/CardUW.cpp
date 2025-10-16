
#include "UIComponents/Cards/CardUW.h"

#include "Components/Button.h"


void UCardUW::SetSelected(bool bIsSelected)
{
}

void UCardUW::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (RootButton) RootButton->OnClicked.AddDynamic(this, &UCardUW::HandleClicked);
}


void UCardUW::HandleClicked()
{
	OnCardSelected.Broadcast(this);
}
