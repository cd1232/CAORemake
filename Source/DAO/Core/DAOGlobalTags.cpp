// This Include
#include "DAOGlobalTags.h"

namespace DAOAbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_MageBolt, "Ability.Skill.MagicBolt");
}

const FGameplayTag& UDAOGlobalTags::Status_Permanent()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Status_Permanent_CanAttack()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.CanAttack")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Status_Changing_IsAlive()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.IsAlive")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Status_Changing_Unarmed()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Unarmed")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Status_Changing_Detector()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Detector")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Status_Changing_Stealthed()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Stealthed")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Status_Changing_Immobilized()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Immobilized")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Status_Changing_Invulnerable()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Invulnerable")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::AbilityActivationFailure_NoTarget()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("AbilityActivationFailure.NoTarget")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Relationship()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Relationship_Self()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Self")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Relationship_Friendly()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Friendly")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Relationship_Hostile()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Hostile")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Relationship_Neutral()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Neutral")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Relationship_SamePlayer()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.SamePlayer")));
	return Tag;
}

const FGameplayTag& UDAOGlobalTags::Relationship_Visible()
{
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Visible")));
	return Tag;
}
