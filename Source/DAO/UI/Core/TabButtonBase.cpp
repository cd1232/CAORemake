// Craig Duthie 2024


#include "TabButtonBase.h"
#include "CommonLazyImage.h"



void UTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void UTabButtonBase::SetTabLabelInfo_Implementation(const FDAOTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}