

#include "UIComponents/Elements/StatsItemUW.h"

#include "Components/TextBlock.h"

void UStatsItemUW::SetText(const FText& NewText) const
{
	if (StatsText)
	{
		StatsText->SetText(NewText);
	}
}