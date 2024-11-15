// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "LyraInventoryItemDefinition.h"

#include "InventoryFragment_EquippableItem.generated.h"

class ULyraEquipmentDefinition;
class UObject;

UCLASS()
class UInventoryFragment_EquippableItem : public ULyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Lyra)
	TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition;
};
