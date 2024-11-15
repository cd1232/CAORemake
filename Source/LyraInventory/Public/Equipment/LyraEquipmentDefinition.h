// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <Templates/SubclassOf.h>

#include "AttributeSet.h"
#include "Equipment/EquipmentSlotType.h"

#include "LyraEquipmentDefinition.generated.h"

class AActor;
class ULyraAbilitySet;
class ULyraEquipmentInstance;
class UGameplayEffect;

USTRUCT()
struct FLyraEquipmentActorToSpawn
{
	GENERATED_BODY()

	FLyraEquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};


/**
 * ULyraEquipmentDefinition
 *
 * Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class LYRAINVENTORY_API ULyraEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	ULyraEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<ULyraEquipmentInstance> InstanceType;

	// Which equipment slots this equipment can be in
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<EEquipmentSlot> EquipmentSlotTypes;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const ULyraAbilitySet>> AbilitySetsToGrant;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FLyraEquipmentActorToSpawn> ActorsToSpawn;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Equipment)
	float Damage = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Equipment)
	TArray<FGameplayAttribute> Attributes;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Equipment)
	float AttributeModifier = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Equipment)
	float ArmorPenetration = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Equipment)
	float SpeedModifier = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Equipment)
	float CriticalChance = 0.02f;
};
