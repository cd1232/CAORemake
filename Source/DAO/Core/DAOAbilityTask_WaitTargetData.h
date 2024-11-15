// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

#include "DAOAbilityTask_WaitTargetData.generated.h"


/**
 * Wait for targeting actor (spawned from parameter) to provide data. Can be set not to end upon outputting data. Can be ended by task name.
 *
 * WARNING: These actors are spawned once per ability activation and in their default form are not very efficient
 * For most games you will need to subclass and heavily modify this actor, or you will want to implement similar functions in a game-specific actor or blueprint to avoid actor spawn costs
 * This task is not well tested by internal games, but it is a useful class to look at to learn how target replication occurs
 */
UCLASS()
class DAO_API UDAOAbilityTask_WaitTargetData: public UAbilityTask_WaitTargetData
{
	GENERATED_UCLASS_BODY()

public:
	/** Spawns target actor and waits for it to return valid data or to be canceled. */
	UFUNCTION(BlueprintCallable, meta=(HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms="Instigator"), Category="Ability|Tasks")
	static UDAOAbilityTask_WaitTargetData* WaitTargetData_DAO(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, TSubclassOf<AGameplayAbilityTargetActor> Class);

	/** Uses specified target actor and waits for it to return valid data or to be canceled. */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UDAOAbilityTask_WaitTargetData* WaitTargetDataUsingActor_DAO(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AGameplayAbilityTargetActor* InTargetActor);

protected:
	virtual void InitializeTargetActor(AGameplayAbilityTargetActor* SpawnedActor) const override;
};