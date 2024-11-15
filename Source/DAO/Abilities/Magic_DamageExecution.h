// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Magic_DamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API UMagic_DamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UMagic_DamageExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	UFUNCTION(BlueprintPure)
	float CalculateBaseDamage() const;
	
};
