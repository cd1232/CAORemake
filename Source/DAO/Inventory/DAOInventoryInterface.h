// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "ItemType.h"
#include "UObject/Interface.h"
#include "DAOInventoryInterface.generated.h"

class ULyraInventoryItemInstance;

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UDAOInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DAO_API IDAOInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual TArray<ULyraInventoryItemInstance*> GetInventoryData() = 0;

	UFUNCTION(BlueprintCallable)
	virtual TArray<ULyraInventoryItemInstance*> GetInventoryDataFiltered(EItemType ItemType) = 0;
};
