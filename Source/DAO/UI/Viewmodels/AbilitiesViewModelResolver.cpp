// Craig Duthie 2024
#include "AbilitiesViewModelResolver.h"

#include "VMAbilities.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "UI/Viewmodels/CharacterInfoInterface.h"

UObject* UAbilitiesViewModelResolver::CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const
{
	Super::CreateInstance(ExpectedType, UserWidget, View);

	if (APlayerController* OwningPC = UserWidget->GetOwningPlayer<APlayerController>())
	{
		if (ICharacterInfoInterface* PlayerInfoInterface = Cast<ICharacterInfoInterface>(OwningPC))
		{
			return PlayerInfoInterface->GetHotbarAbilities();
		}

		return nullptr;
	}

	return nullptr;
}

void UAbilitiesViewModelResolver::DestroyInstance(const UObject* ViewModel, const UMVVMView* View) const
{
	Super::DestroyInstance(ViewModel, View);
}

bool UAbilitiesViewModelResolver::DoesSupportViewModelClass(const UClass* Class) const
{
	return Super::DoesSupportViewModelClass(Class);
}
