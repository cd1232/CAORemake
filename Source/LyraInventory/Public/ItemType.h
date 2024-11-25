#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon,
	Armour,
	Potion,
	Accessory,
	QuestItem,
	Misc,
	MAX UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EItemType, EItemType::MAX);