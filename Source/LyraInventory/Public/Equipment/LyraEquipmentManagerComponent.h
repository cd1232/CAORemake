// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <Components/PawnComponent.h>
#include <Net/Serialization/FastArraySerializer.h>

#include "LyraAbilitySet.h"
#include "Equipment/EquipmentSlotType.h"

#include "LyraEquipmentManagerComponent.generated.h"

class UActorComponent;
class UAbilitySystemComponent;
class ULyraEquipmentDefinition;
class ULyraEquipmentInstance;
class ULyraEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FLyraEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;
class ULyraInventoryItemInstance;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FLyraAppliedEquipmentEntry
{
	GENERATED_BODY()

	FLyraAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

	ULyraEquipmentInstance* GetEquipmentInstance() const { return Instance; }

private:
	friend FLyraEquipmentList;
	friend ULyraEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<ULyraEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FLyraAbilitySet_GrantedHandles GrantedHandles;
};

USTRUCT(BlueprintType)
struct FSlotToEntryPair : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FSlotToEntryPair()
		: EquipmentSlot(EEquipmentSlot::MainHand)
	{}

	FString GetDebugString() const;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEquipmentSlot EquipmentSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FLyraAppliedEquipmentEntry Entry;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FLyraEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FLyraEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FLyraEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FSlotToEntryPair, FLyraEquipmentList>(Entries, DeltaParms, *this);
	}

	ULyraEquipmentInstance* AddEntry(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(ULyraEquipmentInstance* Instance);

	void FillSlotToEntryMap();

private:
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend ULyraEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY(EditAnywhere)
	TArray<FSlotToEntryPair> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FLyraEquipmentList> : public TStructOpsTypeTraitsBase2<FLyraEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};


/**
 * Manages equipment applied to a pawn
 */
UCLASS(BlueprintType, Const)
class LYRAINVENTORY_API ULyraEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	ULyraEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = Inventory, meta = (DisplayName = "CanEquipItem"))
	bool CanEquipItemSlot(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition, EEquipmentSlot SlotType) const;

	UFUNCTION(BlueprintPure, Category = Inventory)
	bool CanEquipItem(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition) const;

	UFUNCTION(BlueprintPure, Category = Inventory)
	bool CanUnequipItem(ULyraEquipmentInstance* EquipmentInstance) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	ULyraEquipmentInstance* EquipItem(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition);

	/* Equips an equipment item from an inventory item instance (if it has an equippable item fragment)*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	ULyraEquipmentInstance* EquipItemFromItemInstance(ULyraInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void UnequipItem(ULyraEquipmentInstance* EquipmentInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ULyraEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<ULyraEquipmentInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<ULyraEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<ULyraEquipmentInstance> InstanceType) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ULyraEquipmentInstance* GetEquipmentInstance(ULyraInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<TSubclassOf<ULyraEquipmentDefinition>> GetEquippedWeapons();

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated, EditAnywhere)
	FLyraEquipmentList EquipmentList;
};


// TODO Craig
// What about equipment that doesn't need to actually be spawned, like rings or necklaces?
// how to track those