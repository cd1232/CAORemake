// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DAOMMC_AbilityCost.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API UDAOMMC_AbilityCost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
