// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/LyraEquipmentManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "Equipment/LyraEquipmentDefinition.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "Net/UnrealNetwork.h"
#include "LyraInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraEquipmentManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

//////////////////////////////////////////////////////////////////////
// FLyraAppliedEquipmentEntry

FString FLyraAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

//////////////////////////////////////////////////////////////////////
// FLyraEquipmentList

void FLyraEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
 	for (int32 Index : RemovedIndices)
 	{
 		const FSlotToEntryPair& SlotPair = Entries[Index];
		if (SlotPair.Entry.Instance != nullptr)
		{
			SlotPair.Entry.Instance->OnUnequipped();
		}
 	}
}

void FLyraEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FSlotToEntryPair& SlotPair = Entries[Index];
		if (SlotPair.Entry.Instance != nullptr)
		{
			SlotPair.Entry.Instance->OnEquipped();
		}
	}
}

void FLyraEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
// 	for (int32 Index : ChangedIndices)
// 	{
// 		const FGameplayTagStack& Stack = Stacks[Index];
// 		TagToCountMap[Stack.Tag] = Stack.StackCount;
// 	}
}

UAbilitySystemComponent* FLyraEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

ULyraEquipmentInstance* FLyraEquipmentList::AddEntry(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition)
{
	ULyraEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
 	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());
	
	const ULyraEquipmentDefinition* EquipmentCDO = GetDefault<ULyraEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<ULyraEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = ULyraEquipmentInstance::StaticClass();
	}
	
	// Entries always exist for every slot, we only change what is inside
	FSlotToEntryPair* ChangedEntry = nullptr;
	for (FSlotToEntryPair& SlotEntry : Entries)
	{
		if (EquipmentCDO->EquipmentSlotTypes.Contains(SlotEntry.EquipmentSlot))
		{		
			SlotEntry.Entry.EquipmentDefinition = EquipmentDefinition;
			SlotEntry.Entry.Instance = NewObject<ULyraEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
			SlotEntry.Entry.Instance->SetEquipmentDef(EquipmentDefinition);

			Result = SlotEntry.Entry.Instance;

			ChangedEntry = &SlotEntry;
		}
	}	

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const ULyraAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &ChangedEntry->Entry.GrantedHandles, Result);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to GetAbilitySystemComponent in FLyraEquipmentList::AddEntry"));
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	MarkItemDirty(*ChangedEntry);

	return Result;
}

void FLyraEquipmentList::RemoveEntry(ULyraEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FSlotToEntryPair& SlotEntry = *EntryIt;
		if (SlotEntry.Entry.Instance == Instance)
		{
			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				SlotEntry.Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}			

			Instance->DestroyEquipmentActors();		

			MarkItemDirty(SlotEntry);
		}
	}
}

void FLyraEquipmentList::FillSlotToEntryMap()
{
	Entries.Reserve((uint8)EEquipmentSlot::MAX);

	// TODO Craig
	// Each character may have a different number of slots
	// So maybe the character can set it up

	for (EEquipmentSlot SlotType : TEnumRange<EEquipmentSlot>())
	{
		FSlotToEntryPair& entry = Entries.AddDefaulted_GetRef();
		entry.EquipmentSlot = SlotType;
	}
}

//////////////////////////////////////////////////////////////////////
// ULyraEquipmentManagerComponent

ULyraEquipmentManagerComponent::ULyraEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;

	EquipmentList.FillSlotToEntryMap();
}

bool ULyraEquipmentManagerComponent::CanEquipItem(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition) const
{
	//EEquipmentSlot equipmentSlotType = EquipmentDefinition->GetDefaultObject<ULyraEquipmentDefinition>()->EquipmentSlotType;

	//const FLyraAppliedEquipmentEntry* foundEntry = EquipmentList.Entries.FindByPredicate([equipmentSlotType](const FLyraAppliedEquipmentEntry& entry){ return entry.GetEquipmentSlotType() == equipmentSlotType; });
	//if (foundEntry)
	//{
	//	// Check if we can unequip the current item in that slot, if so, then we can equip this item
	//	if (CanUnequipItem(foundEntry->GetEquipmentInstance()))
	//	{
	//		
	//	}
	//}
	//else
	//{
	//	// There is no item currently in the slot, just check if we can equip this item

	//}

	// TODO Craig
	return false;
}

bool ULyraEquipmentManagerComponent::CanEquipItemSlot(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition, EEquipmentSlot SlotType) const
{
	ULyraEquipmentDefinition* equipmentCDO = EquipmentDefinition->GetDefaultObject<ULyraEquipmentDefinition>();
	if (!equipmentCDO->EquipmentSlotTypes.Contains(SlotType))
	{
		return false;
	}

	const FSlotToEntryPair* foundEntry = EquipmentList.Entries.FindByPredicate(
	[SlotType](const FSlotToEntryPair& entry) 
	{ return entry.EquipmentSlot == SlotType; });
	
	// Set to true by default in case we don't have equipment in that slot already
	bool bCanUnequip = true;
	if (foundEntry)
	{
		if (foundEntry->Entry.Instance != nullptr)
		{
			// Check if we can unequip the current item in that slot, if so, then we can equip this item
			bCanUnequip = CanUnequipItem(foundEntry->Entry.GetEquipmentInstance());
		}
	}

	// if we made it here for now we can equip
	// TODO Craig - in future we may check if we have a certain class, or certain attribute (enough Strength)
	return true;
}

bool ULyraEquipmentManagerComponent::CanUnequipItem(ULyraEquipmentInstance* EquipmentInstance) const
{
	return true;
	// TODO Craig - in future an equipped item might be a quest object you can't unequip at the moment
}

void ULyraEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

ULyraEquipmentInstance* ULyraEquipmentManagerComponent::EquipItem(TSubclassOf<ULyraEquipmentDefinition> EquipmentClass)
{
	ULyraEquipmentInstance* Result = nullptr;
	if (EquipmentClass != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentClass);
		if (Result != nullptr)
		{
			Result->OnEquipped();

			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}
	return Result;
}

ULyraEquipmentInstance* ULyraEquipmentManagerComponent::EquipItemFromItemInstance(ULyraInventoryItemInstance* ItemInstance)
{
	ULyraEquipmentInstance* newEquipment = EquipItem(ItemInstance->GetEquipmentDefinition());
	if (newEquipment)
	{
		newEquipment->SetInstigator(ItemInstance);
	}

	return newEquipment;
}

void ULyraEquipmentManagerComponent::UnequipItem(ULyraEquipmentInstance* EquipmentInstance)
{
	if (EquipmentInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(EquipmentInstance);
		}

		EquipmentInstance->OnUnequipped();
		EquipmentList.RemoveEntry(EquipmentInstance);
	}
}

bool ULyraEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FSlotToEntryPair& SlotEntry : EquipmentList.Entries)
	{
		ULyraEquipmentInstance* Instance = SlotEntry.Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void ULyraEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void ULyraEquipmentManagerComponent::UninitializeComponent()
{
	TArray<ULyraEquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FSlotToEntryPair& SlotEntry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(SlotEntry.Entry.Instance);
	}

	for (ULyraEquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

void ULyraEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing LyraEquipmentInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FSlotToEntryPair& SlotEntry : EquipmentList.Entries)
		{
			ULyraEquipmentInstance* Instance = SlotEntry.Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

ULyraEquipmentInstance* ULyraEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<ULyraEquipmentInstance> InstanceType)
{
	for (FSlotToEntryPair& SlotEntry : EquipmentList.Entries)
	{
		if (ULyraEquipmentInstance* Instance = SlotEntry.Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<ULyraEquipmentInstance*> ULyraEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<ULyraEquipmentInstance> InstanceType) const
{
	TArray<ULyraEquipmentInstance*> Results;
	for (const FSlotToEntryPair& SlotEntry : EquipmentList.Entries)
	{
		if (ULyraEquipmentInstance* Instance = SlotEntry.Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}


ULyraEquipmentInstance* ULyraEquipmentManagerComponent::GetEquipmentInstance(ULyraInventoryItemInstance* ItemInstance)
{
	for (const FSlotToEntryPair& SlotEntry : EquipmentList.Entries)
	{
		if (ULyraEquipmentInstance* Instance = SlotEntry.Entry.Instance)
		{
			ULyraInventoryItemInstance* itemInstance = Cast<ULyraInventoryItemInstance>(Instance->GetInstigator());
			if (itemInstance == ItemInstance)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<TSubclassOf<ULyraEquipmentDefinition>> ULyraEquipmentManagerComponent::GetEquippedWeapons()
{
	TArray<TSubclassOf<ULyraEquipmentDefinition>> EquippedWeapons;

	for (const FSlotToEntryPair& SlotEntry : EquipmentList.Entries)
	{
		if (SlotEntry.EquipmentSlot == EEquipmentSlot::MainHand || SlotEntry.EquipmentSlot == EEquipmentSlot::OffHand)
		{
			if (SlotEntry.Entry.Instance)
			{
				EquippedWeapons.Add(SlotEntry.Entry.EquipmentDefinition);
											
			}
		}
	}

	return EquippedWeapons;
}

TSubclassOf<ULyraEquipmentDefinition> ULyraEquipmentManagerComponent::GetEquippedItemDefInSlot(EEquipmentSlot Slot)
{
	for (const FSlotToEntryPair& SlotEntry : EquipmentList.Entries)
	{
		if (SlotEntry.EquipmentSlot == Slot)
		{
			if (SlotEntry.Entry.Instance)
			{
				return SlotEntry.Entry.EquipmentDefinition;											
			}
		}
	}

	return nullptr;
}

ULyraEquipmentInstance* ULyraEquipmentManagerComponent::GetEquippedItemInSlot(EEquipmentSlot Slot)
{
	for (const FSlotToEntryPair& SlotEntry : EquipmentList.Entries)
	{
		if (SlotEntry.EquipmentSlot == Slot)
		{
			return SlotEntry.Entry.Instance;
		}
	}

	return nullptr;
}

FString FSlotToEntryPair::GetDebugString() const
{
	return FString(TEXT("SlotToEntryPair"));
	//return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}
