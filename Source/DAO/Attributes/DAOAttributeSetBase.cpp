// Craig Duthie 2023

// This Include
#include "DAOAttributeSetBase.h"

// Engine Includes
#include <Net/UnrealNetwork.h>

#include "Abilities/DAOAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOAttributeSetBase)

void UDAOAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOAttributeSetBase, Health, OldHealth);
}

void UDAOAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UDAOAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOAttributeSetBase, Mana, OldMana);
}

void UDAOAttributeSetBase::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOAttributeSetBase, MaxMana, OldMaxMana);
}

void UDAOAttributeSetBase::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOAttributeSetBase, Armor, OldArmor);
}

void UDAOAttributeSetBase::OnRep_MagicResistance(const FGameplayAttributeData& OldMagicResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOAttributeSetBase, MagicResistance, OldMagicResistance);
}

void UDAOAttributeSetBase::OnRep_ExpGranted(const FGameplayAttributeData& OldExpGranted)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOAttributeSetBase, ExpGranted, OldExpGranted);
}

void UDAOAttributeSetBase::OnRep_ExpToNextLevel(const FGameplayAttributeData& OldExpToNextLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOAttributeSetBase, ExpToNextLevel, OldExpToNextLevel);
}

void UDAOAttributeSetBase::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOAttributeSetBase, CharacterLevel, OldCharacterLevel);
}

void UDAOAttributeSetBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDAOAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOAttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOAttributeSetBase, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOAttributeSetBase, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOAttributeSetBase, MagicResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOAttributeSetBase, ExpGranted, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOAttributeSetBase, CharacterLevel, COND_None, REPNOTIFY_Always);
}

UDAOAbilitySystemComponent* UDAOAttributeSetBase::GetDAOAbilitySystemComponent() const
{
	return Cast<UDAOAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

bool UDAOAttributeSetBase::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// Handle modifying incoming normal damage
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			//const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);

			//if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
			//{
			//	// Do not take away any health.
			//	Data.EvaluatedData.Magnitude = 0.0f;
			//	return false;
			//}

#if !UE_BUILD_SHIPPING
			// Check GodMode cheat, unlimited health is checked below
			//if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode) && !bIsDamageFromSelfDestruct)
			//{
			//	// Do not take away any health.
			//	Data.EvaluatedData.Magnitude = 0.0f;
			//	return false;
			//}
#endif // #if !UE_BUILD_SHIPPING
		}
	}

	// Save the current health
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	return true;
}

void UDAOAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	//const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);

	float MinimumHealth = 0.0f;

#if !UE_BUILD_SHIPPING
	// Godmode and unlimited health stop death unless it's a self destruct
	//if (!bIsDamageFromSelfDestruct &&
	//	(Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode) || Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_UnlimitedHealth)))
	//{
	//	MinimumHealth = 1.0f;
	//}
#endif // #if !UE_BUILD_SHIPPING

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		// Send a standardized verb message that other systems can observe
		//if (Data.EvaluatedData.Magnitude > 0.0f)
		//{
		//	FLyraVerbMessage Message;
		//	Message.Verb = TAG_Lyra_Damage_Message;
		//	Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
		//	Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
		//	Message.Target = GetOwningActor();
		//	Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
		//	//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
		//	//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
		//	Message.Magnitude = Data.EvaluatedData.Magnitude;

		//	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
		//	MessageSystem.BroadcastMessage(Message.Verb, Message);
		//}

		// Convert into -Health and then clamp
		SetHealth(FMath::Clamp(GetHealth() - GetDamageTaken(), MinimumHealth, GetMaxHealth()));
		SetDamageTaken(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// TODO clamp current health?

		// Notify on any requested max health changes
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// If health has actually changed activate callbacks
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// Check health again in case an event above changed it.
	bOutOfHealth = (GetHealth() <= 0.0f);

}

void UDAOAttributeSetBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UDAOAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UDAOAttributeSetBase::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetMaxHealthAttribute())
	{
		// Make sure current health is not greater than the new max health.
		if (GetHealth() > NewValue)
		{
			UDAOAbilitySystemComponent* ASC = GetDAOAbilitySystemComponent();
			check(ASC);

			ASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UDAOAttributeSetBase::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
