// Craig Duthie 2023

#pragma once

// Engine Includes
#include <Engine/AssetManager.h>


// Generated Include
#include "DAOAssetManager.generated.h"

class UDAOItemData;

/**
 * 
 */
UCLASS()
class DAO_API UDAOAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	/** Returns the current AssetManager object */
	static UDAOAssetManager& Get();

	virtual void StartInitialLoading() override;

public:
	/** Static types for items */
	static const FPrimaryAssetType	PotionItemType;
	static const FPrimaryAssetType	SkillItemType;
	static const FPrimaryAssetType	TokenItemType;

	static const FPrimaryAssetType	WeaponItemType;
	static const FPrimaryAssetType	HelmetItemType;
	static const FPrimaryAssetType	LegsItemType;
	static const FPrimaryAssetType	BootsItemType;
	static const FPrimaryAssetType	GauntletsItemType;


};
