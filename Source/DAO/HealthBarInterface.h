// Craig Duthie 2023

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthBarInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHealthBarInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DAO_API IHealthBarInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(float CurrentHealth, float MaxHealth);
};