// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "View/MVVMViewModelContextResolver.h"
#include "PlayerInfoViewModelResolver.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API UPlayerInfoViewModelResolver : public UMVVMViewModelContextResolver
{
	GENERATED_BODY()
	
public:
	virtual UObject* CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const override;
	virtual void DestroyInstance(const UObject* ViewModel, const UMVVMView* View) const override;
	virtual bool DoesSupportViewModelClass(const UClass* Class) const override;
};
