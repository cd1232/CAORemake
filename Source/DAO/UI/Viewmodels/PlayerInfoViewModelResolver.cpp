// Craig Duthie 2024


#include "UI/Viewmodels/PlayerInfoViewModelResolver.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "UI/Viewmodels/CharacterInfoInterface.h"

UObject* UPlayerInfoViewModelResolver::CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const
{
	Super::CreateInstance(ExpectedType, UserWidget, View);

	if (APlayerController* OwningPC = UserWidget->GetOwningPlayer<APlayerController>())
	{
		if (ICharacterInfoInterface* PlayerInfoInterface = Cast<ICharacterInfoInterface>(OwningPC))
		{
			return PlayerInfoInterface->GetCharacterInfo();
		}

		return nullptr;
	}

	return nullptr;
}

void UPlayerInfoViewModelResolver::DestroyInstance(const UObject* ViewModel, const UMVVMView* View) const
{
	Super::DestroyInstance(ViewModel, View);
}

bool UPlayerInfoViewModelResolver::DoesSupportViewModelClass(const UClass* Class) const
{
	return Super::DoesSupportViewModelClass(Class);
}
