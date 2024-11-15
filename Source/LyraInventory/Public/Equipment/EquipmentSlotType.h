#pragma once

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	MainHand,
	OffHand,
	RangedAmmo,
	Chest,
	Head,
	Boots,
	Gloves,
	Ring1,
	Ring2,
	Neck,
	Belt,
	MAX UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EEquipmentSlot, EEquipmentSlot::MAX);
