// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "DAOButtonBase.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class DAO_API UDAOButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);
	
protected:
	// UUserWidget interface
	virtual void NativePreConstruct() override;
	// End of UUserWidget interface

	// UCommonButtonBase interface
	virtual void UpdateInputActionWidget() override;
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;
	virtual void NativeOnCurrentTextStyleChanged() override;
	// End of UCommonButtonBase interface

	void RefreshButtonText();
	
	
	void UpdateButtonText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();
	
private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> TextBlock_ButtonText;
	
	UPROPERTY(EditAnywhere, Category="Button", meta=(InlineEditConditionToggle))
	uint8 bOverride_ButtonText : 1;
	
	UPROPERTY(EditAnywhere, Category="Button", meta=( editcondition="bOverride_ButtonText" ))
	FText ButtonText;
};
