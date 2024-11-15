// Craig Duthie 2023

#pragma once

// Engine Includes
#include <CommonGameInstance.h>

// Local Includes

// Generated Include
#include "DAOGameInstance.generated.h"

// Forward Declares
class UDAOSaveGame;
class USaveGame;

/**
 * 
 */
UCLASS()
class DAO_API UDAOGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:
	// Constructor
	UDAOGameInstance(const FObjectInitializer& ObjectInitializer);

	/** Handle the final setup required after loading a USaveGame object using AsyncLoadGameFromSlot. Returns true if it loaded, false if it created one */
	UFUNCTION(BlueprintCallable, Category = Save)
	bool HandleSaveGameLoaded(USaveGame* SaveGameObject);

	/** Returns the current save game, so it can be used to initialize state. Changes are not written until WriteSaveGame is called */
	UFUNCTION(BlueprintCallable, Category = Save)
	UDAOSaveGame* GetCurrentSaveGame();

	/** Returns true if this is a valid inventory slot */
	//UFUNCTION(BlueprintCallable, Category = Inventory)
	//bool IsValidItemSlot(FDAOItemSlot ItemSlot) const;

	/** Writes the current save game object to disk. The save to disk happens in a background thread*/
	UFUNCTION(BlueprintCallable, Category = Save)
	bool WriteSaveGame();

	/** Gets the save game slot and user index used for inventory saving, ready to pass to GameplayStatics save functions */
	UFUNCTION(BlueprintCallable, Category = Save)
	void GetSaveSlotInfo(FString& SlotName, int32& UserIndex) const;

protected:
	/** Called when the async save happens */
	virtual void HandleAsyncSave(const FString& SlotName, const int32 UserIndex, bool bSuccess);

public:
	// /** Delegate called when the save game has been loaded/reset */
	// UPROPERTY(BlueprintAssignable)
	// FOnSaveGameLoaded OnSaveGameLoaded;
	//
	// /** Native delegate for save game load/reset */
	// FOnSaveGameLoadedNative OnSaveGameLoadedNative;

protected:
	/** The current save game object */
	UPROPERTY()
	UDAOSaveGame* CurrentSaveGame;

	/** Rather it will attempt to actually save to disk */
	UPROPERTY()
	bool bSavingEnabled;

	/** True if we are in the middle of doing a save */
	UPROPERTY()
	bool bCurrentlySaving;

	/** True if another save was requested during a save */
	UPROPERTY()
	bool bPendingSaveRequested;

	/** The slot name used for saving */
	UPROPERTY(BlueprintReadWrite, Category = Save)
	FString SaveSlot;

	/** The platform-specific user index */
	UPROPERTY(BlueprintReadWrite, Category = Save)
	int32 SaveUserIndex;
	
};
