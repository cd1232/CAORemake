// Craig Duthie 2023

#pragma once

// Engine Includes
#include <CommonPlayerController.h>
#include "InputActionValue.h"

// Local Includes
#include "ItemType.h"
#include "Inventory/DAOInventoryInterface.h"
#include "UI/Viewmodels/CharacterInfoInterface.h"
#include "RTSMoveOrder.h"
#include "RTSAttackOrder.h"
#include "RTSUseAbilityOrder.h"
#include "RTSOrderData.h"

// Generated Include
#include "DAOPlayerController.generated.h"

// Forward Declares
class ADAOCharacter;
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UNiagaraComponent;
class ADAOBaseCharacter;
class UActionDataAsset;
class UDAOItemData;
class UDAOSaveGame;
class ULyraInventoryItemInstance;
class UVMPlayerInfo;
class UTargetingPreset;
class ARTSOrderAIController;
class UVMAbilities;

USTRUCT(BlueprintType)
struct FPartyCharacterInfo
{
	GENERATED_BODY()

public:
	/* Might be null if not spawned in the world */
	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<ADAOCharacter> Character;
	
};

DECLARE_LOG_CATEGORY_EXTERN(LogDAOCharacter, Log, All);

UCLASS()
class ADAOPlayerController : public ACommonPlayerController, public IDAOInventoryInterface, public ICharacterInfoInterface
{
	GENERATED_BODY()

public:
	ADAOPlayerController();

	UFUNCTION(BlueprintCallable)
	void SetControlledCharacter(ADAOBaseCharacter* NewCharacter);

	UFUNCTION(BlueprintPure)
	ADAOBaseCharacter* GetControlledCharacter() const;

	// Implement IRPGInventoryInterface
	virtual TArray<ULyraInventoryItemInstance*> GetInventoryData() override;
	virtual TArray<ULyraInventoryItemInstance*> GetInventoryDataFiltered(EItemType ItemType) override;
	// Implement IRPGInventoryInterface

	// Implement ICharacterInfoInterface
	virtual UVMPlayerInfo* GetCharacterInfo() const override;
	virtual UVMAbilities* GetHotbarAbilities() const override;
	// Implement ICharacterInfoInterface

	UFUNCTION(BlueprintCallable)
	void SelectAbilityForCasting(TSoftClassPtr<UGameplayAbility> AbilityToCast);

	void SetMouseCursor(EMouseCursor::Type CursorType, bool bLockMouseCursor = false);
	void SetMouseCursorLock(bool bLock);
	
	FTraceHandle GetHitResultAtScreenPositionAsync(const FVector2D ScreenPosition, const ECollisionChannel TraceChannel, bool bTraceComplex, FTraceDelegate* InDelegate) const;

	UFUNCTION(BlueprintPure)
	TArray<UTexture2D*> GetCurrentPartyIcons();

	
protected:
	// To add mapping context
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;


	UFUNCTION()
	void OnControlledCharacterPossessed();

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();

	void OnMouseRotationTriggered(const FInputActionValue& Value);
	void OnConfirmTargetTriggered(const FInputActionValue& Value);
	void OnCancelTargetTriggered(const FInputActionValue& Value);

	void OnAbilityHotkeyTriggered(const UInputAction* TriggeringAction, int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
	void StartCameraRotation();

	UFUNCTION(BlueprintImplementableEvent)
	void StopCameraRotation();

	UFUNCTION(BlueprintCallable)
	void IssueOrder(const FRTSOrderData& Order);

	/* Calls StopMovement but also destroys the nav decal if it still exists */
	UFUNCTION(BlueprintCallable)
	void StopMovementAndDestroyDecal();

	UFUNCTION(BlueprintPure)
	bool IsAlive() const;

	/** Calls the inventory update callbacks */
	// void NotifyInventoryItemChanged(bool bAdded, UDAOItemData* Item);
	//
	// void NotifySlottedItemChanged(FDAOItemSlot ItemSlot, UDAOItemData* Item);
	//
	// void NotifyInventoryLoaded();

	/** Manually save the inventory, this is called from add/remove functions automatically */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SaveInventory();

	/** Loads inventory from save game on game instance, this will replace arrays */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool LoadInventory();

	/** Called when a global save game as been loaded */
	void HandleSaveGameLoaded(UDAOSaveGame* NewSaveGame);

	/** Fills in any empty slots with items in inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void FillEmptySlots();

	void StartTargeting();

public:
	// /** Delegate called when an inventory item has been added or removed */
	// UPROPERTY(BlueprintAssignable, Category = Inventory)
	// FOnInventoryItemChanged OnInventoryItemChanged;
	//
	// /** Native version above, called before BP delegate */
	// FOnInventoryItemChangedNative OnInventoryItemChangedNative;
	//
	// /** Delegate called when an inventory slot has changed */
	// UPROPERTY(BlueprintAssignable, Category = Inventory)
	// FOnSlottedItemChanged OnSlottedItemChanged;
	//
	// /** Native version above, called before BP delegate */
	// FOnSlottedItemChangedNative OnSlottedItemChangedNative;
	//
	// /** Delegate called when the inventory has been loaded/reloaded */
	// UPROPERTY(BlueprintAssignable, Category = Inventory)
	// FOnInventoryLoaded OnInventoryLoaded;
	//
	// /** Native version above, called before BP delegate */
	// FOnInventoryLoadedNative OnInventoryLoadedNative;

protected:
	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UNiagaraSystem> FXCursor;

private:
	TWeakObjectPtr<ADAOBaseCharacter> CurrentlyControlledCharacter;
	TWeakObjectPtr<ARTSOrderAIController> CharacterAIController;

	TWeakObjectPtr<UNiagaraComponent> SpawnedNavDecal;

	FVector CachedDestination = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	FVector2D StartingMousePosition = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	float MinDifferenceBeforeStartingRotation = 5.0f;

#pragma region Input Mappings
/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> GenericUIMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SetDestinationClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> RotateCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MouseRotateCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ConfirmTargetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CancelTargetAction;

	/* All actions bound from 1-0 with - and = */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Abilities, meta=(AllowPrivateAccess = "true"))
	TArray<TObjectPtr<const UInputAction>> AbilityHotkeyActions;
#pragma endregion	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Abilities, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<URTSAttackOrder> AttackOrder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Abilities, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<URTSMoveOrder> MoveOrder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Abilities, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<URTSUseAbilityOrder> UseAbilityOrder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UTargetingPreset> InteractionTargetingPreset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TSoftObjectPtr<ADAOCharacter>> PartyCharacters;

	UPROPERTY(BlueprintReadOnly, Category = ViewModel, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UVMPlayerInfo> CharacterInfoViewModel;

	UPROPERTY(BlueprintReadOnly, Category = ViewModel, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UVMAbilities> AbilitiesViewModel;

	FRTSOrderData OrderDataForTargeting;

	FVector2D TrackedMouseDelta = FVector2D::ZeroVector;
	FVector2D DifferenceSinceTrackingMouse = FVector2D::ZeroVector;
	bool bHasMovedEnoughToStartRotation = false;

	/* If we have found a valid point to move to */
	bool bHasGotMovementPoint = false;

	bool bHasRotatedCamera = false;

	bool bSelectingTargetForCast = false;
	bool bIsMouseCursorLocked = false;
};


