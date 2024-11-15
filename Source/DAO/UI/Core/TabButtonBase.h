// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "DAOButtonBase.h"
#include "DAOTabListWidget.h"
#include "TabButtonBase.generated.h"

class UCommonLazyImage;

/**
 * 
 */
UCLASS()
class DAO_API UTabButtonBase : public UDAOButtonBase, public IDAOTabButtonInterface
{
	GENERATED_BODY()

public:
	void SetIconBrush(const FSlateBrush& Brush);

protected:

	UFUNCTION()
	virtual void SetTabLabelInfo_Implementation(const FDAOTabDescriptor& TabLabelInfo) override;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
};
