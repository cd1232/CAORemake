// Craig Duthie 2023

// This Include
#include "Player/DAOPlayerState.h"

// Engine Includes
#include <GameplayTagContainer.h>

// Local Includes
#include "Abilities/DAOAbilitySystemComponent.h"
#include "Attributes/DAOAttributeSetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOPlayerState)

ADAOPlayerState::ADAOPlayerState()
{
	SetNetUpdateFrequency(100.0f);
}

void ADAOPlayerState::ShowAbilityConfirmCancelText(bool bShouldShowText)
{
	// TODO Craig
}