// Craig Duthie 2023

// This Include
#include "DAOGameInstance.h"

// Engine Includes
#include <GameFramework/SaveGame.h>
#include <Kismet/GameplayStatics.h>

// Local Includes
#include "DAOSaveGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOGameInstance)

UDAOGameInstance::UDAOGameInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

bool UDAOGameInstance::HandleSaveGameLoaded(USaveGame* SaveGameObject)
{
	bool bLoaded = false;

	if (!bSavingEnabled)
	{
		// If saving is disabled, ignore passed in object
		SaveGameObject = nullptr;
	}

	// Replace current save, old object will GC out
	CurrentSaveGame = Cast<UDAOSaveGame>(SaveGameObject);

	if (CurrentSaveGame)
	{
		// Make sure it has any newly added default inventory
		//AddDefaultInventory(CurrentSaveGame, false);
		bLoaded = true;
	}
	else
	{
		// This creates it on demand
		CurrentSaveGame = Cast<UDAOSaveGame>(UGameplayStatics::CreateSaveGameObject(UDAOSaveGame::StaticClass()));

		//AddDefaultInventory(CurrentSaveGame, true);
	}

	// OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	// OnSaveGameLoadedNative.Broadcast(CurrentSaveGame);

	return bLoaded;
}

UDAOSaveGame* UDAOGameInstance::GetCurrentSaveGame()
{
	return CurrentSaveGame;
}

//bool UDAOGameInstance::IsValidItemSlot(FDAOItemSlot ItemSlot) const
//{
//	if (ItemSlot.IsValid())
//	{
//		const int32* FoundCount = ItemSlotsPerType.Find(ItemSlot.EquipmentSlotType);
//
//		if (FoundCount)
//		{
//			return ItemSlot.SlotNumber < *FoundCount;
//		}
//	}
//	return false;
//}

bool UDAOGameInstance::WriteSaveGame()
{
	if (bSavingEnabled)
	{
		if (bCurrentlySaving)
		{
			// Schedule another save to happen after current one finishes. We only queue one save
			bPendingSaveRequested = true;
			return true;
		}

		// Indicate that we're currently doing an async save
		bCurrentlySaving = true;

		// This goes off in the background
		UGameplayStatics::AsyncSaveGameToSlot(GetCurrentSaveGame(), SaveSlot, SaveUserIndex, FAsyncSaveGameToSlotDelegate::CreateUObject(this, &UDAOGameInstance::HandleAsyncSave));
		return true;
	}
	return false;
}

void UDAOGameInstance::GetSaveSlotInfo(FString& SlotName, int32& UserIndex) const
{
	SlotName = SaveSlot;
	UserIndex = SaveUserIndex;
}

void UDAOGameInstance::HandleAsyncSave(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	ensure(bCurrentlySaving);
	bCurrentlySaving = false;

	if (bPendingSaveRequested)
	{
		// Start another save as we got a request while saving
		bPendingSaveRequested = false;
		WriteSaveGame();
	}
}
