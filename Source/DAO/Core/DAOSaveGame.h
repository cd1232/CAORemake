// Craig Duthie 2023

#pragma once

// Engine Includes
#include "GameFramework/SaveGame.h"

// Local Includes

// Generated Include
#include "DAOSaveGame.generated.h"

/** List of versions, native code will handle fixups for any old versions */
namespace EDAOSaveGameVersion
{
	enum type
	{
		// Initial version
		Initial,

		// -----<new versions must be added before this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};
}

/** Object that is written to and read from the save game archive, with a data version */
UCLASS(BlueprintType)
class DAO_API UDAOSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Constructor */
	UDAOSaveGame()
	{
		// Set to current version, this will get overwritten during serialization when loading
		SavedDataVersion = EDAOSaveGameVersion::LatestVersion;
	}

	// /** Map of items to item data */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	// TMap<FPrimaryAssetId, FDAOItemInventoryData> InventoryData;
	//
	// /** Map of slotted items */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	// TMap<FDAOItemSlot, FPrimaryAssetId> SlottedItems;

	/** User's unique id */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	FString UserId;

protected:
	/** Overridden to allow version fixups */
	virtual void Serialize(FArchive& Ar) override;

private:
	/** What LatestVersion was when the archive was saved */
	UPROPERTY()
	int32 SavedDataVersion;
};

