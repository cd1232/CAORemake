// Craig Duthie 2023

// This Include
#include "DAOBaseCharacter.h"

// Engine Includes

// Local Includes
#include "CharacterInfoDataAsset.h"
#include "Abilities/DAOAbilitySystemComponent.h"
#include "Attributes/DAOAttributeSetBase.h"
#include "Attributes/DAOPrimaryAttributeSet.h"
#include "Player/DAOPlayerState.h"
#include "DAOGlobalTags.h"
#include "DAOCollisionTypes.h"
#include "InventoryFragment_EquippableItem.h"
#include "LyraInventoryManagerComponent.h"
#include "Equipment/LyraEquipmentManagerComponent.h"
#include "RTSOrderComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOBaseCharacter)

// Sets default values
ADAOBaseCharacter::ADAOBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.RemoveOnDeath"));

	AbilitySystemComponent = CreateDefaultSubobject<UDAOAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UDAOAttributeSetBase>(TEXT("AttributeSet"));
	PrimaryAttributeSet = CreateDefaultSubobject<UDAOPrimaryAttributeSet>(TEXT("PrimaryAttributeSet"));

	OrderComponent = CreateDefaultSubobject<URTSOrderComponent>(TEXT("OrderComponent"));

	LyraInventoryComponentNew = CreateDefaultSubobject<ULyraInventoryManagerComponent>(TEXT("LyraInventoryComponentNew"));
	LyraEquipmentComponent = CreateDefaultSubobject<ULyraEquipmentManagerComponent>(TEXT("LyraEquipmentComponent"));
}

bool ADAOBaseCharacter::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void ADAOBaseCharacter::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent->bHaveAbilitiesBeenGiven)
	{
		return;
	}

	AbilitySystemComponent->ClearAllAbilities();
	AbilitySystemComponent->bHaveAbilitiesBeenGiven = false;
}

void ADAOBaseCharacter::Die()
{
	RemoveCharacterAbilities();
	UE_LOG(LogTemp, Log, TEXT("Character Die"));

	// TODO Craig
	// Disable Collisions + Gravity? Ragdoll?
	GetMesh()->SetCollisionProfileName(DAOCollisionProfile::NPCDead);

	OnCharacterDied.Broadcast(this);

	AbilitySystemComponent->CancelAbilities();

	FGameplayTagContainer EffectTagsToRemove;
	EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);

	AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);
	AbilitySystemComponent->AddReplicatedLooseGameplayTag(DeadTag);	

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void ADAOBaseCharacter::FinishDying()
{

}

float ADAOBaseCharacter::GetHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetHealth();
	}

	return 0.0f;
}

float ADAOBaseCharacter::GetMaxHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxHealth();
	}

	return 0.0f;

}

float ADAOBaseCharacter::GetMana() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMana();
	}

	return 0.0f;
}

float ADAOBaseCharacter::GetMaxMana() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxMana();
	}

	return 0.0f;
}

float ADAOBaseCharacter::GetArmor() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetArmor();
	}

	return 0.0f;
}

float ADAOBaseCharacter::GetMagicResistance() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMagicResistance();
	}

	return 0.0f;
}

float ADAOBaseCharacter::GetCharacterLevel() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetCharacterLevel();
	}

	return 0.0f;
}

FText ADAOBaseCharacter::GetCharacterName() const
{
	return CharacterName;
}

UTexture2D* ADAOBaseCharacter::GetCharacterIcon() const
{
	return CharacterIcon;
}

void ADAOBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeStartingValues();

	AddStartupEffects();

	BindASC();

	OnCharacterPossessed.Broadcast();
}

URTSOrderComponent* ADAOBaseCharacter::GetRTSOrderComponent() const
{
	return OrderComponent;
}

void ADAOBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AttributeSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
		AttributeSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
		AttributeSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ADAOBaseCharacter::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ADAOBaseCharacter::MaxHealthChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).AddUObject(this, &ADAOBaseCharacter::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute()).AddUObject(this, &ADAOBaseCharacter::MaxManaChanged);
		ArmorChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetArmorAttribute()).AddUObject(this, &ADAOBaseCharacter::ArmorChanged);
		MagicResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMagicResistanceAttribute()).AddUObject(this, &ADAOBaseCharacter::MagicResistanceChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCharacterLevelAttribute()).AddUObject(this, &ADAOBaseCharacter::LevelChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Status.Changing.Immobilized")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ADAOBaseCharacter::StunTagChanged);
	}

	UCharacterInfoDataAsset* LoadedCharacterInfo = CharacterInfoAsset.LoadSynchronous();
	if (LoadedCharacterInfo)
	{
		GetMesh()->SetSkeletalMesh(LoadedCharacterInfo->SkeletalMesh.LoadSynchronous());
		GetMesh()->SetAnimInstanceClass(LoadedCharacterInfo->AnimInstance.LoadSynchronous());
		CharacterIcon = LoadedCharacterInfo->CharacterIcon.LoadSynchronous();
		CharacterTag = LoadedCharacterInfo->CharacterTag;

		for (FDefaultInventoryItem& Item : LoadedCharacterInfo->DefaultInventory)
		{
			TSubclassOf<ULyraInventoryItemDefinition> ItemDefClass = Item.ItemDefinition.LoadSynchronous();
			ULyraInventoryItemInstance* ItemInstance = LyraInventoryComponentNew->AddItemDefinition(ItemDefClass);

			if (Item.bIsEquipped)
			{					
				LyraEquipmentComponent->EquipItemFromItemInstance(ItemInstance);	
			}
		}
	}
}

void ADAOBaseCharacter::InitializeAttributes()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("Default Attributes not set on Character"));
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1.0f, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}
}

void ADAOBaseCharacter::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.Get() || AbilitySystemComponent->bHaveAbilitiesBeenGiven)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1.0f, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
		}
	}

	AbilitySystemComponent->bHaveStartupEffectsBeenApplied = true;
}

void ADAOBaseCharacter::InitializeStartingValues()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// TODO Craig
	// Probably don't want to do this if we're just switching characters
	AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
	
	InitializeAttributes();

	SetHealth(GetMaxHealth());
	SetMana(GetMaxMana());

	if (GetHealth() > 0.0f)
	{
		AbilitySystemComponent->AddTag(UDAOGlobalTags::Status_Changing_IsAlive());
		AbilitySystemComponent->AddTag(UDAOGlobalTags::Status_Permanent_CanAttack());
	}

	bAbilitiesInitialized = true;
}

void ADAOBaseCharacter::BindASC()
{
	if (!bHaveBoundASCInput && AbilitySystemComponent && IsValid(InputComponent))
	{


		bHaveBoundASCInput = true;
	}
}

void ADAOBaseCharacter::SetHealth(float Health)
{
	if (AttributeSet)
	{
		AttributeSet->SetHealth(Health);
	}
}

void ADAOBaseCharacter::SetMana(float Mana)
{
	if (AttributeSet)
	{
		AttributeSet->SetMana(Mana);
	}
}

UAbilitySystemComponent* ADAOBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UDAOAbilitySystemComponent* ADAOBaseCharacter::GetDAOAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADAOBaseCharacter::SetOwningPlayerController(APlayerController* PC)
{
	OwningPlayerController = PC;
}

APlayerController* ADAOBaseCharacter::GetOwningPlayerController() const
{
	return OwningPlayerController.Get();
}

const TArray<TSubclassOf<UGameplayAbility>>& ADAOBaseCharacter::GetAbilityHotkeyAbilities() const
{
	return AbilityHotkeyAbilities;
}

void ADAOBaseCharacter::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(OldValue, NewValue);
}

void ADAOBaseCharacter::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	UE_LOG(LogTemp, Log, TEXT("MaxHealthChanged"));

}

void ADAOBaseCharacter::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	UE_LOG(LogTemp, Log, TEXT("Out of health (<=0)"));
	
	if (NewValue <= 0.0f)
	{
		Die();
	}
}

void ADAOBaseCharacter::HealthChanged(const FOnAttributeChangeData& Data)
{

}

void ADAOBaseCharacter::MaxHealthChanged(const FOnAttributeChangeData& Data)
{

}

void ADAOBaseCharacter::ManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("ManaChanged"));
}

void ADAOBaseCharacter::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("Max Mana Changed"));
}

void ADAOBaseCharacter::ArmorChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("ArmorChanged"));
}

void ADAOBaseCharacter::MagicResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("MagicResistanceChanged"));
}

void ADAOBaseCharacter::LevelChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("LevelChanged"));
}

void ADAOBaseCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag("Ability"));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag("Ability.NotCanceledByStun"));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}
