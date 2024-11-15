// Craig Duthie 2023

// This Include
#include "DAOAssetManager.h"

// Engine Includes
#include <AbilitySystemGlobals.h>

// Local Includes

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOAssetManager)

const FPrimaryAssetType	UDAOAssetManager::PotionItemType = TEXT("Potion");
const FPrimaryAssetType	UDAOAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType	UDAOAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType	UDAOAssetManager::WeaponItemType = TEXT("Weapon");

const FPrimaryAssetType	UDAOAssetManager::HelmetItemType = TEXT("Helmet");
const FPrimaryAssetType	UDAOAssetManager::LegsItemType = TEXT("Legs");
const FPrimaryAssetType	UDAOAssetManager::BootsItemType = TEXT("Boots");
const FPrimaryAssetType	UDAOAssetManager::GauntletsItemType = TEXT("Gauntlet");

UDAOAssetManager& UDAOAssetManager::Get()
{
	UDAOAssetManager* This = Cast<UDAOAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be RPGAssetManager!"));
		return *NewObject<UDAOAssetManager>(); // never calls this
	}
}

void UDAOAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();

	UE_LOG(LogTemp, Log, TEXT("UDAOAssetManager::StartInitialLoading"));
}
