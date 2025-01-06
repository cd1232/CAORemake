// Craig Duthie 2023

#pragma once

// Engine Includes
#include <GameFramework/Character.h>
#include <AbilitySystemInterface.h>
#include <GameplayTagContainer.h>
#include <GameplayEffectTypes.h>
#include <GameplayAbilitySpec.h>

// Local Includes
#include "Inventory/DAOInventoryInterface.h"
#include "RTSGameplayAbility.h"

// Generated Include
#include "DAOBaseCharacter.generated.h"

class UCharacterInfoDataAsset;
// Forward Declares
class UDAOAbilitySystemComponent;
class UDAOAttributeSetBase;
class UAnimMontage;
class UGameplayEffect;
class UDAOPrimaryAttributeSet;
class UDAOItemData;
class UInventoryComponent;
class ULyraEquipmentManagerComponent;
class ULyraInventoryManagerComponent;
class UVMPlayerInfo;
class URTSOrderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ADAOBaseCharacter*, Character);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterAttributeChangedDelegate, float, OldValue, float, NewValue);

DECLARE_MULTICAST_DELEGATE(FCharacterPossessedDelegate);

UCLASS()
class DAO_API ADAOBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADAOBaseCharacter();

	UFUNCTION(BlueprintCallable, Category = "Character|Attributes")
	bool IsAlive() const;

	void RemoveCharacterAbilities();

	void Die();

	void FinishDying();
	
	UFUNCTION(BlueprintPure, Category = "Character|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Character|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Character|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintPure, Category = "Character|Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintPure, Category = "Character|Attributes")
	float GetArmor() const;

	UFUNCTION(BlueprintPure, Category = "Character|Attributes")
	float GetMagicResistance() const;

	UFUNCTION(BlueprintPure, Category = "Character|Attributes")
	float GetCharacterLevel() const;

	UFUNCTION(BlueprintPure, Category = "Character")
	FText GetCharacterName() const;

	UFUNCTION(BlueprintPure, Category = "Character")
	UTexture2D* GetCharacterIcon() const;

	// ACharacter
	virtual void PossessedBy(AController* NewController) override;
	// ACharacter

	UFUNCTION(BlueprintPure, Category = "Character|Actions")
	URTSOrderComponent* GetRTSOrderComponent() const;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UDAOAbilitySystemComponent* GetDAOAbilitySystemComponent() const;

	void SetOwningPlayerController(APlayerController* PC);
	APlayerController* GetOwningPlayerController() const;

	UFUNCTION(BlueprintPure, Category = "Abilities")
	const TArray<TSubclassOf<UGameplayAbility>>& GetAbilityHotkeyAbilities() const;

protected:
	virtual void BeginPlay() override;

	void InitializeAttributes();

	void AddStartupEffects();

	void InitializeStartingValues();

	void BindASC();

	void SetHealth(float Health);

	void SetMana(float Mana);

private:
	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

	void HealthChanged(const FOnAttributeChangeData& Data);
	void MaxHealthChanged(const FOnAttributeChangeData& Data);
	void ManaChanged(const FOnAttributeChangeData& Data);
	void MaxManaChanged(const FOnAttributeChangeData& Data);
	void ArmorChanged(const FOnAttributeChangeData& Data);
	void MagicResistanceChanged(const FOnAttributeChangeData& Data);
	void LevelChanged(const FOnAttributeChangeData& Data);

	void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

public:
	UPROPERTY(BlueprintAssignable)
	FCharacterDiedDelegate OnCharacterDied;

	FCharacterPossessedDelegate OnCharacterPossessed;

	UPROPERTY(BlueprintAssignable)
	FCharacterAttributeChangedDelegate OnHealthChanged;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle ArmorChangedDelegateHandle;
	FDelegateHandle MagicResistanceChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
	FText CharacterName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character|Attributes")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character|Attributes")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	/* Abilities sitting on the quickbar */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Abilities, meta=(AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UGameplayAbility>> AbilityHotkeyAbilities;

	/* This will be valid if the player controller is currently controlling this character (but GetController will still return an AI one) */
	TWeakObjectPtr<APlayerController> OwningPlayerController;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDAOAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULyraEquipmentManagerComponent> LyraEquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULyraInventoryManagerComponent> LyraInventoryComponentNew;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCharacterInfoDataAsset> CharacterInfoAsset;
	
	UPROPERTY()
	TObjectPtr<UDAOAttributeSetBase> AttributeSet;

	UPROPERTY()
	TObjectPtr<UDAOPrimaryAttributeSet> PrimaryAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URTSOrderComponent> OrderComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTexture2D> CharacterIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayTag CharacterTag;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	/** Delegate handles */
	FDelegateHandle InventoryUpdateHandle;
	FDelegateHandle InventoryLoadedHandle;

	bool bHaveBoundASCInput = false;

	bool bAbilitiesInitialized = false;
};
