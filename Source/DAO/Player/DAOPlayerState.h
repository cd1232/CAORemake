// Craig Duthie 2023

#pragma once

// Engine Includes
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"

// Generated Include
#include "DAOPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API ADAOPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ADAOPlayerState();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ShowAbilityConfirmCancelText(bool bShouldShowText);
};
