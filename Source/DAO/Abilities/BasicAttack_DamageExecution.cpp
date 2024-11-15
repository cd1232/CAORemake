// Craig Duthie 2023

// This Include
#include "BasicAttack_DamageExecution.h"

// Engine Includes
#include <AbilitySystemComponent.h>
#include <GameplayEffectTypes.h>

// Local Includes
#include "Attributes/DAOPrimaryAttributeSet.h"
#include "Attributes/DAOAttributeSetBase.h"
#include "Equipment/LyraEquipmentManagerComponent.h"
#include "Equipment/LyraEquipmentDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BasicAttack_DamageExecution)

struct FDamageStatics
{
	// Source
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Dexterity);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Cunning);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CharacterLevel);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);


	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDAOAttributeSetBase, CharacterLevel, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDAOPrimaryAttributeSet, Strength, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDAOPrimaryAttributeSet, Cunning, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDAOPrimaryAttributeSet, Dexterity, Source, false);


		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UDAOAttributeSetBase::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UDAOAttributeSetBase, Armor, Target, false);
	}
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DmgStatics;
	return DmgStatics;
}

UBasicAttack_DamageExecution::UBasicAttack_DamageExecution()
{
	// Source
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().DexterityDef);
	RelevantAttributesToCapture.Add(DamageStatics().CunningDef);
	RelevantAttributesToCapture.Add(DamageStatics().CharacterLevelDef);
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);

	// Target
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UBasicAttack_DamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	float BaseWeaponDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluationParameters, BaseWeaponDamage);
	
	float Strength = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StrengthDef, EvaluationParameters, Strength);

	float Dexterity = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DexterityDef, EvaluationParameters, Dexterity);

	float Cunning = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CunningDef, EvaluationParameters, Cunning);

	TArray<TSubclassOf<ULyraEquipmentDefinition>> equippedWeapons = SourceActor->FindComponentByClass<ULyraEquipmentManagerComponent>()->GetEquippedWeapons();
	
	ULyraEquipmentDefinition* equippedWeaponCDO = nullptr;
	if (equippedWeapons.Num() > 0)
	{
		equippedWeaponCDO = equippedWeapons[0]->GetDefaultObject<ULyraEquipmentDefinition>();
	}

	const float AttributeModifier = equippedWeaponCDO ? equippedWeaponCDO->AttributeModifier : 1.0f;
	const float WeaponStyleModifier = 0.625f; // TODO Craig
	const float AttributeBasedDamage = (Strength - 10) * AttributeModifier * WeaponStyleModifier;

	float CharacterLevel = 1.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CharacterLevelDef, EvaluationParameters, CharacterLevel);
	const float CharacterDamageBonus = 0.4f * CharacterLevel; 

	float OnHitDamage = 0.0f;
	const float AttackerArmorPenetration = equippedWeaponCDO ? (equippedWeaponCDO->ArmorPenetration + ((Cunning - 10) / 7)) : 0.0f;

	// Target
	float TargetArmorValue = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmorValue);

	const float TargetArmorDamageReduction = FMath::Max((0.7f + 0.3 * FMath::RandRange(0.0f, 1.0f) * TargetArmorValue - AttackerArmorPenetration), 0.0f);

	// TODO Craig
	// Get Critical damage modifier from attacker
	const float CriticalDamageModifier = 1.5f;

	const float RandCritCheck = FMath::RandRange(1, 100);
	float DamageDone = 1.0f;
	if (RandCritCheck <= (equippedWeaponCDO ? equippedWeaponCDO->CriticalChance : 5.0f))
	{
		// Critical Hit
		DamageDone = FMath::Max(CriticalDamageModifier * (BaseWeaponDamage + AttributeBasedDamage) + CharacterDamageBonus - TargetArmorDamageReduction + OnHitDamage - TargetArmorDamageReduction, 0.0f);
	}
	else
	{
		// Normal Hit
		DamageDone = FMath::Max(BaseWeaponDamage + AttributeBasedDamage + CharacterDamageBonus - TargetArmorDamageReduction + OnHitDamage - TargetArmorDamageReduction, 0.0f);

	}

	// Clamping is done when damage is converted to -health
	if (DamageDone > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UDAOAttributeSetBase::GetDamageTakenAttribute(), EGameplayModOp::Additive, DamageDone));
	}
}
