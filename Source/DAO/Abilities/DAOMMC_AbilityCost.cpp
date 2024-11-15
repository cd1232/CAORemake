// Craig Duthie 2024


#include "Abilities/DAOMMC_AbilityCost.h"

#include "RTSGameplayAbility.h"

float UDAOMMC_AbilityCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	if (!Ability)
	{
		return 0.0f;
	}

	return Ability->GetCost().GetValueAtLevel(Ability->GetAbilityLevel());
}

