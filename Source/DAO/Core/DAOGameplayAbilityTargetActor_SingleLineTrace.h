// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "DAOGameplayAbilityTargetActor_Trace.h"

#include "DAOGameplayAbilityTargetActor_SingleLineTrace.generated.h"

UCLASS(Blueprintable)
class DAO_API ADAOGameplayAbilityTargetActor_SingleLineTrace : public ADAOGameplayAbilityTargetActor_Trace
{
	GENERATED_UCLASS_BODY()

public:
	virtual bool IsConfirmTargetingAllowed() override;

protected:
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
};
