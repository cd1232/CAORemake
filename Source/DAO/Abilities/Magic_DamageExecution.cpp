// Craig Duthie 2024


#include "Abilities/Magic_DamageExecution.h"

// Engine Includes
#include <AbilitySystemComponent.h>
#include <GameplayEffectTypes.h>

// Local Includes
#include "Attributes/DAOPrimaryAttributeSet.h"
#include "Attributes/DAOAttributeSetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Magic_DamageExecution)

struct FMagicDamageStatics
{
	// Source
	DECLARE_ATTRIBUTE_CAPTUREDEF(Magic);

	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FMagicDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDAOPrimaryAttributeSet, Magic, Source, true);
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UDAOAttributeSetBase::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static const FMagicDamageStatics& MagicDamageStatics()
{
	static FMagicDamageStatics DmgStatics;
	return DmgStatics;
}

UMagic_DamageExecution::UMagic_DamageExecution()
{
	// Source
	RelevantAttributesToCapture.Add(MagicDamageStatics().MagicDef);
	RelevantAttributesToCapture.Add(MagicDamageStatics().BaseDamageDef);

	//Target
}


float UMagic_DamageExecution::CalculateBaseDamage() const
{
	return 1.0f;
}

void UMagic_DamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MagicDamageStatics().BaseDamageDef, EvaluationParameters, BaseDamage);

	float Magic = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MagicDamageStatics().MagicDef, EvaluationParameters, Magic);

	float AttackerDamageModifier = 1.0f;
	
	// Target
	float TargetResistanceModifier = 1.0f;	

	float DamageDone = BaseDamage * AttackerDamageModifier * TargetResistanceModifier;

	// Clamping is done when damage is converted to -health
	if (DamageDone > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UDAOAttributeSetBase::GetDamageTakenAttribute(), EGameplayModOp::Additive, DamageDone));
	}
}


