#include "DAOPrimaryAttributeSet.h"

// Engine Includes
#include "Net/UnrealNetwork.h"

void UDAOPrimaryAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOPrimaryAttributeSet, Strength, OldStrength);

}

void UDAOPrimaryAttributeSet::OnRep_Dexterity(const FGameplayAttributeData& OldDexterity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOPrimaryAttributeSet, Dexterity, OldDexterity);

}

void UDAOPrimaryAttributeSet::OnRep_Willpower(const FGameplayAttributeData& OldWillpower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOPrimaryAttributeSet, Willpower, OldWillpower);
}

void UDAOPrimaryAttributeSet::OnRep_Magic(const FGameplayAttributeData& OldMagic)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOPrimaryAttributeSet, Magic, OldMagic);
}

void UDAOPrimaryAttributeSet::OnRep_Cunning(const FGameplayAttributeData& OldCunning)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOPrimaryAttributeSet, Cunning, OldCunning);
}

void UDAOPrimaryAttributeSet::OnRep_Constitution(const FGameplayAttributeData& OldConstitution)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDAOPrimaryAttributeSet, Constitution, OldConstitution);
}

void UDAOPrimaryAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDAOPrimaryAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOPrimaryAttributeSet, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOPrimaryAttributeSet, Willpower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOPrimaryAttributeSet, Magic, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOPrimaryAttributeSet, Cunning, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDAOPrimaryAttributeSet, Constitution, COND_None, REPNOTIFY_Always);
}
