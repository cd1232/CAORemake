#pragma once


// Engine Includes
#include <AttributeSet.h>
#include <AbilitySystemComponent.h>

// Generated Include
#include "DAOPrimaryAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class DAO_API UDAOPrimaryAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnRep_Strength(const FGameplayAttributeData& OldStrength);

	UFUNCTION()
	virtual void OnRep_Dexterity(const FGameplayAttributeData& OldDexterity);

	UFUNCTION()
	virtual void OnRep_Willpower(const FGameplayAttributeData& OldWillpower);

	UFUNCTION()
	virtual void OnRep_Magic(const FGameplayAttributeData& OldMagic);

	UFUNCTION()
	virtual void OnRep_Cunning(const FGameplayAttributeData& OldCunning);

	UFUNCTION()
	virtual void OnRep_Constitution(const FGameplayAttributeData& OldConstitution);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Strength", ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UDAOPrimaryAttributeSet, Strength)

	UPROPERTY(BlueprintReadOnly, Category = "Dexterity", ReplicatedUsing = OnRep_Dexterity)
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(UDAOPrimaryAttributeSet, Dexterity)

	UPROPERTY(BlueprintReadOnly, Category = "Willpower", ReplicatedUsing = OnRep_Willpower)
	FGameplayAttributeData Willpower;
	ATTRIBUTE_ACCESSORS(UDAOPrimaryAttributeSet, Willpower)

	UPROPERTY(BlueprintReadOnly, Category = "Magic", ReplicatedUsing = OnRep_Magic)
	FGameplayAttributeData Magic;
	ATTRIBUTE_ACCESSORS(UDAOPrimaryAttributeSet, Magic)

	UPROPERTY(BlueprintReadOnly, Category = "Cunning", ReplicatedUsing = OnRep_Cunning)
	FGameplayAttributeData Cunning;
	ATTRIBUTE_ACCESSORS(UDAOPrimaryAttributeSet, Cunning)

	UPROPERTY(BlueprintReadOnly, Category = "Constitution", ReplicatedUsing = OnRep_Constitution)
	FGameplayAttributeData Constitution;
	ATTRIBUTE_ACCESSORS(UDAOPrimaryAttributeSet, Constitution)

};