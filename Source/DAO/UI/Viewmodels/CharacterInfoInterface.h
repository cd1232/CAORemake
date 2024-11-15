// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VMPlayerInfo.h"

#include "CharacterInfoInterface.generated.h"

class UVMPlayerInfo;
class UVMAbilities;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterInfoInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DAO_API ICharacterInfoInterface
{
	GENERATED_BODY()

public:
	virtual UVMPlayerInfo* GetCharacterInfo() const = 0;	

	virtual UVMAbilities* GetHotbarAbilities() const = 0;
};