// Craig Duthie 2024

#pragma once

// Engine Includes
#include <AttributeSet.h>
#include <AbilitySystemComponent.h>
#include "GameplayEffectExtension.h"

// Generated Include
#include "DAOAttributeSetBase.generated.h"

class UDAOAbilitySystemComponent;

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

	/**
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients:
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_SixParams(FDAOAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);


/**
 * 
 */
UCLASS()
class DAO_API UDAOAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	virtual void OnRep_MagicResistance(const FGameplayAttributeData& OldMagicResistance);

	UFUNCTION()
	virtual void OnRep_ExpGranted(const FGameplayAttributeData& OldExpGranted);

	UFUNCTION()
	virtual void OnRep_ExpToNextLevel(const FGameplayAttributeData& OldExpToNextLevel);

	UFUNCTION()
	virtual void OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UDAOAbilitySystemComponent* GetDAOAbilitySystemComponent() const;

protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, Health)

	UPROPERTY(BlueprintReadOnly, Category = "MaxHealth", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, Mana)

	UPROPERTY(BlueprintReadOnly, Category = "MaxMana", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, MaxMana)

	UPROPERTY(BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, Armor)

	UPROPERTY(BlueprintReadOnly, Category = "MagicResistance", ReplicatedUsing = OnRep_MagicResistance)
	FGameplayAttributeData MagicResistance;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, MagicResistance)

	UPROPERTY(BlueprintReadOnly, Category = "ExpGranted", ReplicatedUsing = OnRep_ExpGranted)
	FGameplayAttributeData ExpGranted;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, ExpGranted)

	UPROPERTY(BlueprintReadOnly, Category = "ExpToNextLevel", ReplicatedUsing = OnRep_ExpToNextLevel)
	FGameplayAttributeData ExpToNextLevel;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, ExpToNextLevel)

	UPROPERTY(BlueprintReadOnly, Category = "CharacterLevel", ReplicatedUsing = OnRep_CharacterLevel)
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, CharacterLevel)

	UPROPERTY(BlueprintReadOnly, Category = "ExpGranted")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, ArmorPenetration)

	// Meta attribute used for damage execution
	// Base Damage that you will use when attacking something
	UPROPERTY(BlueprintReadOnly, Category = "BaseDamage")
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, BaseDamage)

	// This attribute will be set when you are hit by something
	UPROPERTY(BlueprintReadOnly, Category = "DamageTaken")
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UDAOAttributeSetBase, DamageTaken)

	// Used to track when the health reaches 0.
	bool bOutOfHealth = false;

	// Store the health before any changes 
	float MaxHealthBeforeAttributeChange = 0.0f;
	float HealthBeforeAttributeChange = 0.0f;

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FDAOAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FDAOAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FDAOAttributeEvent OnOutOfHealth;
};
