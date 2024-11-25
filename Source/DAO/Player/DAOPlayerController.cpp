// Craig Duthie 2023

// This Include
#include "DAOPlayerController.h"

// Engine Includes
#include <GameFramework/Pawn.h>
#include <NiagaraFunctionLibrary.h>
#include <Engine/World.h>
#include <EnhancedInputComponent.h>
#include <InputActionValue.h>
#include <EnhancedInputSubsystems.h>
#include <Engine/LocalPlayer.h>
#include <NiagaraComponent.h>
#include <AbilitySystemComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/HUD.h>
#include "Abilities/GameplayAbility.h"

// Local Includes
#include "DAOCharacter.h"
#include "InteractionComponent.h"
#include "Core/DAOBaseCharacter.h"
#include "Core/RelationshipComponent.h"
#include "Core/DAOGlobalTags.h"
#include "Core/DAOGameInstance.h"
#include "Core/DAOSaveGame.h"
#include "Core/DAOAssetManager.h"
#include "LyraInventoryManagerComponent.h"
#include "Abilities/DAOAbilitySystemComponent.h"
#include "RTSOrderData.h"
#include "RTSOrderAIController.h"
#include "RTSOrderHelper.h"
#include "RTSOrderComponent.h"
#include "UI/Viewmodels/VMAbilities.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOPlayerController)

DEFINE_LOG_CATEGORY(LogDAOCharacter);

ADAOPlayerController::ADAOPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	CharacterInfoViewModel = CreateDefaultSubobject<UVMPlayerInfo>(TEXT("PlayerInfo"));
	AbilitiesViewModel = CreateDefaultSubobject<UVMAbilities>(TEXT("AbilitiesVM"));
}

void ADAOPlayerController::SetControlledCharacter(ADAOBaseCharacter* NewCharacter)
{
	// If we are switching controlled character should we unbind from OnCharacterPossessed?
	if (CurrentlyControlledCharacter.IsValid())
	{
		CurrentlyControlledCharacter.Get()->OnCharacterPossessed.RemoveAll(this);
		CurrentlyControlledCharacter.Get()->SetOwningPlayerController(nullptr);
		CharacterAIController.Reset();
	}

	CurrentlyControlledCharacter = NewCharacter;

	if (CurrentlyControlledCharacter.IsValid())
	{
		CurrentlyControlledCharacter->SetOwningPlayerController(this);

		// Set up the view model with the new character data
		CharacterInfoViewModel->SetCurrentHealth(CurrentlyControlledCharacter->GetHealth());
		CharacterInfoViewModel->SetMaxHealth(CurrentlyControlledCharacter->GetMaxHealth());
		CharacterInfoViewModel->SetCurrentMana(CurrentlyControlledCharacter->GetMana());
		CharacterInfoViewModel->SetMaxMana(CurrentlyControlledCharacter->GetMaxMana());
		CharacterInfoViewModel->SetPlayerName(CurrentlyControlledCharacter->GetCharacterName());
		
		const TArray<TSubclassOf<UGameplayAbility>>& hotbarAbilities = CurrentlyControlledCharacter->GetAbilityHotkeyAbilities();
		AbilitiesViewModel->AddAbilities(hotbarAbilities);

		// TODO Craig - Setup delegates to update the view model when health/mana/character changes
		//CurrentlyControlledCharacter->HealthChangedDelegateHandle.
		PartyCharacters.AddUnique(CurrentlyControlledCharacter.Get());
	}

	if (NewCharacter && NewCharacter->GetController())
	{
		OnControlledCharacterPossessed();
	}
	else if (NewCharacter)
	{
		NewCharacter->OnCharacterPossessed.AddUObject(this, &ADAOPlayerController::OnControlledCharacterPossessed);
	}

}

ADAOBaseCharacter* ADAOPlayerController::GetControlledCharacter() const
{
	return CurrentlyControlledCharacter.Get();
}

UVMPlayerInfo* ADAOPlayerController::GetCharacterInfo() const
{
	return CharacterInfoViewModel;
}

UVMAbilities* ADAOPlayerController::GetHotbarAbilities() const
{
	return AbilitiesViewModel;
}

void ADAOPlayerController::SelectAbilityForCasting(TSoftClassPtr<UGameplayAbility> AbilityToCast)
{
	if (CurrentlyControlledCharacter.IsValid())
	{
		if (AbilityToCast.IsValid())
		{
			if (UDAOAbilitySystemComponent* ASC = CurrentlyControlledCharacter.Get()->GetDAOAbilitySystemComponent())
			{
				OrderDataForTargeting.OrderType = UseAbilityOrder;
				OrderDataForTargeting.Index = ASC->GetAbilityIndex(AbilityToCast.LoadSynchronous());

				StartTargeting();
			}
		}
	}
}

void ADAOPlayerController::SetMouseCursor(EMouseCursor::Type CursorType, bool bLockMouseCursor)
{
	if (!bIsMouseCursorLocked || bLockMouseCursor)
	{
		CurrentMouseCursor = CursorType;
	}
}

void ADAOPlayerController::SetMouseCursorLock(bool bLock)
{
	bIsMouseCursorLocked = bLock;
}

TArray<ULyraInventoryItemInstance*> ADAOPlayerController::GetInventoryData()
{
	// TODO Craig
	// We also want to return the global (unequipped/not-character-specific inventory) here
	if (CurrentlyControlledCharacter.IsValid())
	{		
		if (const ULyraInventoryManagerComponent* CharacterInventory = CurrentlyControlledCharacter->FindComponentByClass<ULyraInventoryManagerComponent>())
		{
			return CharacterInventory->GetAllItems();
		}
	}

	return {};
}

TArray<ULyraInventoryItemInstance*> ADAOPlayerController::GetInventoryDataFiltered(EItemType ItemType)
{
	if (CurrentlyControlledCharacter.IsValid())
	{		
		if (const ULyraInventoryManagerComponent* CharacterInventory = CurrentlyControlledCharacter->FindComponentByClass<ULyraInventoryManagerComponent>())
		{
			return CharacterInventory->GetAllItemsOfType(ItemType);
		}
	}

	return {};
}

void ADAOPlayerController::BeginPlay()
{  
	// Load inventory off save game before starting play
	LoadInventory();

	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
		Subsystem->AddMappingContext(GenericUIMappingContext, 1);
	}
}

void ADAOPlayerController::OnControlledCharacterPossessed()
{
	if (CurrentlyControlledCharacter.IsValid() && CurrentlyControlledCharacter->GetController())
	{
		CharacterAIController = Cast<ARTSOrderAIController>(CurrentlyControlledCharacter->GetController());
	}
}

void ADAOPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ADAOPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ADAOPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ADAOPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ADAOPlayerController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(MouseRotateCameraAction, ETriggerEvent::Triggered, this, &ADAOPlayerController::OnMouseRotationTriggered);

		int32 index = 0;
		for (const UInputAction* action : AbilityHotkeyActions)
		{
			EnhancedInputComponent->BindAction(action, ETriggerEvent::Triggered, this, &ADAOPlayerController::OnAbilityHotkeyTriggered, action, index);
			++index;
		}

		EnhancedInputComponent->BindAction(ConfirmTargetAction, ETriggerEvent::Triggered, this, &ADAOPlayerController::OnConfirmTargetTriggered);
		EnhancedInputComponent->BindAction(CancelTargetAction, ETriggerEvent::Triggered, this, &ADAOPlayerController::OnCancelTargetTriggered);
	}
	else
	{
		UE_LOG(LogDAOCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ADAOPlayerController::OnInputStarted()
{
	StopMovement();
	GetMousePosition(StartingMousePosition.X, StartingMousePosition.Y);
}

// Triggered every frame when the input is held down
void ADAOPlayerController::OnSetDestinationTriggered()
{
	// If it was a short press
	if (bHasMovedEnoughToStartRotation)
	{
		// Start camera rotation of player
		bShowMouseCursor = false;
		bHasRotatedCamera = true;
		StartCameraRotation();
	}	
}

void ADAOPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	bHasMovedEnoughToStartRotation = false;

	if (!bHasRotatedCamera && IsAlive())
	{
		// We look for the location in the world where the player has pressed the input
		FHitResult Hit;

		//Only move if we aren't targeting an enemy/interactable, otherwise let that deal with it?
		bool bHitSuccessful = GetHitResultUnderCursorByChannel(TraceTypeQuery3, false, Hit);
		if (bHitSuccessful)
		{
			UInteractionComponent* InteractionComponent = FindComponentByClass<UInteractionComponent>();
			if (InteractionComponent)
			{
				InteractionComponent->ProcessInteractable(Hit.GetActor());
				//UTargetingSubsystem* TargetingSubsystem = GetGameInstance()->GetSubsystem<UTargetingSubsystem>();
				//FTargetingSourceContext TargetingSourceContext;
				//TargetingSourceContext.SourceActor = this;
				//TargetingSourceContext.InstigatorActor = this;

				//FTargetingRequestHandle InteractionTargetHandle = TargetingSubsystem->MakeTargetRequestHandle(InteractionTargetingPreset, TargetingSourceContext);

				//TargetingSubsystem->ExecuteTargetingRequestWithHandle(InteractionTargetHandle);

				//TArray<FHitResult> Targets;
				//TargetingSubsystem->GetTargetingResults(InteractionTargetHandle, Targets);

				ADAOBaseCharacter* SelectedCharacter = InteractionComponent->GetSelectedCharacter();
				if (SelectedCharacter)
				{
					// TODO Craig
					// if relationship is hostile, attack? 
					if (URelationshipComponent* RelationshipComponent = SelectedCharacter->FindComponentByClass<URelationshipComponent>())
					{
						if (RelationshipComponent->GetRelationshipTag().MatchesTag(UDAOGlobalTags::Relationship_Hostile()))
						{
							FRTSOrderData OrderData;
							OrderData.OrderType = AttackOrder;
							OrderData.Target = SelectedCharacter;
							
							URTSOrderHelper::IssueOrder(CurrentlyControlledCharacter.Get(), OrderData);
						}
					}
				}

			}
		}
		else
		{
			// TODO Craig
			// Pass through nullptr to unselect?
			if (UInteractionComponent* InteractionComponent = FindComponentByClass<UInteractionComponent>())
			{
				InteractionComponent->ProcessInteractable(nullptr);
			}

			bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
			// If we hit a surface, cache the location
			if (bHitSuccessful)
			{
				bHasGotMovementPoint = true;
				CachedDestination = Hit.Location;
			}
			
			FRTSOrderData OrderData;
			OrderData.OrderType = MoveOrder;
			OrderData.bUseLocation = true;
			OrderData.Location = CachedDestination;

			URTSOrderHelper::IssueOrder(CurrentlyControlledCharacter.Get(), OrderData);

			if (SpawnedNavDecal.IsValid())
			{
				SpawnedNavDecal->DestroyInstance();
			}

			SpawnedNavDecal = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f), true, true, ENCPoolMethod::None, true);
	}
	}

	bShowMouseCursor = true;
	bHasGotMovementPoint = false;
	bHasRotatedCamera = false;
	StopCameraRotation();
}

void ADAOPlayerController::OnMouseRotationTriggered(const FInputActionValue& Value)
{
	DifferenceSinceTrackingMouse = StartingMousePosition + TrackedMouseDelta;
	TrackedMouseDelta = Value.Get<FVector2D>();

	if (DifferenceSinceTrackingMouse.X >= MinDifferenceBeforeStartingRotation ||
		DifferenceSinceTrackingMouse.Y >= MinDifferenceBeforeStartingRotation)
	{
		bHasMovedEnoughToStartRotation = true;
		// We have moved enough to start rotating for the entire time the mouse is down, reset on mouse up.
	}
}

void ADAOPlayerController::OnConfirmTargetTriggered(const FInputActionValue& Value)
{
	if (CurrentlyControlledCharacter.IsValid() && bSelectingTargetForCast)
	{
		FHitResult HitResult;
		if (GetHitResultUnderCursorByChannel(TraceTypeQuery3, false, HitResult))
		{
			UE_LOG(LogTemp, Log, TEXT("ConfirmTarget"));
			
			OrderDataForTargeting.Target = HitResult.GetActor();
			CurrentlyControlledCharacter->GetRTSOrderComponent()->IssueOrder(OrderDataForTargeting);

			// if (bWasIssued)
			// {
			// 	SetMouseCursorLock(false);
			// 	SetMouseCursor(EMouseCursor::Type::Default);
			// }
			// TODO Craig
			// Getting first actor at the moment
			// How do we know if order was successful??
		}
	}
}

void ADAOPlayerController::OnCancelTargetTriggered(const FInputActionValue& Value)
{
	if (CurrentlyControlledCharacter.IsValid() && bSelectingTargetForCast)
	{
		bSelectingTargetForCast = false;
		OrderDataForTargeting = FRTSOrderData();
		// TODO Craig
		// Cancel targeting

		//CurrentlyControlledCharacter->GetAbilitySystemComponent()->LocalInputCancel();
	}
}

void ADAOPlayerController::OnAbilityHotkeyTriggered(const UInputAction* TriggeringAction, int32 Index)
{
	if (CurrentlyControlledCharacter.IsValid())
	{
		if (UDAOAbilitySystemComponent* ASC = CurrentlyControlledCharacter.Get()->GetDAOAbilitySystemComponent())
		{
			TSubclassOf<UGameplayAbility> GameplayAbilityToCast = CurrentlyControlledCharacter->GetAbilityHotkeyAbilities()[Index];
			if (GameplayAbilityToCast)
			{
				OrderDataForTargeting.OrderType = UseAbilityOrder;
				OrderDataForTargeting.Index = ASC->GetAbilityIndex(GameplayAbilityToCast);
				
				StartTargeting();
			}
		}
	}
}

FTraceHandle ADAOPlayerController::GetHitResultAtScreenPositionAsync(const FVector2D ScreenPosition, const ECollisionChannel TraceChannel, bool bTraceComplex, FTraceDelegate* InDelegate) const
{
	FTraceHandle TraceHandle;
	// Early out if we clicked on a HUD hitbox
	if (GetHUD() != NULL && GetHUD()->GetHitBoxAtCoordinates(ScreenPosition, true))
	{
		return TraceHandle;
	}

	FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), bTraceComplex);
	
	FVector WorldOrigin;
	FVector WorldDirection;
	if (UGameplayStatics::DeprojectScreenToWorld(this, ScreenPosition, WorldOrigin, WorldDirection) == true)
	{
		TraceHandle = GetWorld()->AsyncLineTraceByChannel(EAsyncTraceType::Single, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, TraceChannel, CollisionQueryParams,
			FCollisionResponseParams::DefaultResponseParam, InDelegate);
	}

	return TraceHandle;
}

TArray<UTexture2D*> ADAOPlayerController::GetCurrentPartyIcons()
{
	TArray<UTexture2D*> Icons;
	for (int i = 0; i < PartyCharacters.Num(); i++)
	{
		Icons.Add(PartyCharacters[i].Get()->GetCharacterIcon());
	}

	return Icons;
}

void ADAOPlayerController::IssueOrder(const FRTSOrderData& Order)
{
	if (!Order.OrderType.IsValid())
	{
		Order.OrderType.LoadSynchronous();
	}

	URTSOrderHelper::IssueOrder(CurrentlyControlledCharacter.Get(), Order);
}

void ADAOPlayerController::StopMovementAndDestroyDecal()
{
	if (CharacterAIController.IsValid())
	{
		CharacterAIController->StopMovement();
	}

	if (SpawnedNavDecal.IsValid())
	{
		SpawnedNavDecal->DestroyInstance();
	}

	// No longer interacting with anything if we move
	if (UInteractionComponent* InteractionComponent = FindComponentByClass<UInteractionComponent>())
	{
		InteractionComponent->ProcessInteractable(nullptr);
	}
}

bool ADAOPlayerController::IsAlive() const
{
	if (CurrentlyControlledCharacter.IsValid())
	{
		return CurrentlyControlledCharacter.Get()->IsAlive();
	}

	return false;
}

// void ADAOPlayerController::NotifyInventoryItemChanged(bool bAdded, UDAOItemData* Item)
// {
// 	// Notify native before blueprint
// 	OnInventoryItemChangedNative.Broadcast(bAdded, Item);
// 	OnInventoryItemChanged.Broadcast(bAdded, Item);
//
// 	// Call BP update event
// 	InventoryItemChanged(bAdded, Item);
// }
//
// void ADAOPlayerController::NotifySlottedItemChanged(FDAOItemSlot ItemSlot, UDAOItemData* Item)
// {
// 	// Notify native before blueprint
// 	OnSlottedItemChangedNative.Broadcast(ItemSlot, Item);
// 	OnSlottedItemChanged.Broadcast(ItemSlot, Item);
//
// 	// Call BP update event
// 	SlottedItemChanged(ItemSlot, Item);
// }
//
// void ADAOPlayerController::NotifyInventoryLoaded()
// {
// 	// Notify native before blueprint
// 	OnInventoryLoadedNative.Broadcast();
// 	OnInventoryLoaded.Broadcast();
// }

bool ADAOPlayerController::SaveInventory()
{
	UWorld* World = GetWorld();
	UDAOGameInstance* GameInstance = World ? World->GetGameInstance<UDAOGameInstance>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	UDAOSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	if (CurrentSaveGame)
	{
		// Reset cached data in save game before writing to it
		// CurrentSaveGame->InventoryData.Reset();
		// CurrentSaveGame->SlottedItems.Reset();

		/*for (const TPair<UDAOItem*, FDAOItemInventoryData>& ItemPair : InventoryData)
		{
			FPrimaryAssetId AssetId;

			if (ItemPair.Key)
			{
				AssetId = ItemPair.Key->GetPrimaryAssetId();
				CurrentSaveGame->InventoryData.Add(AssetId, ItemPair.Value);
			}
		}*/

		// TODO Craig
		/*for (const TPair<FDAOItemSlot, UDAOItem*>& SlotPair : SlottedItems)
		{
			FPrimaryAssetId AssetId;

			if (SlotPair.Value)
			{
				AssetId = SlotPair.Value->GetPrimaryAssetId();
			}
			CurrentSaveGame->SlottedItems.Add(SlotPair.Key, AssetId);
		}*/

		// Now that cache is updated, write to disk
		GameInstance->WriteSaveGame();
		return true;
	}
	return false;
}

bool ADAOPlayerController::LoadInventory()
{
	//InventoryData.Reset();
	//SlottedItems.Reset();

	// Fill in slots from game instance
	UWorld* World = GetWorld();
	UDAOGameInstance* GameInstance = World ? World->GetGameInstance<UDAOGameInstance>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	// Bind to loaded callback if not already bound
	// if (!GameInstance->OnSaveGameLoadedNative.IsBoundToObject(this))
	// {
	// 	GameInstance->OnSaveGameLoadedNative.AddUObject(this, &ADAOPlayerController::HandleSaveGameLoaded);
	// }
	
	// TODO Craig
	//for (const TPair<FPrimaryAssetType, int32>& Pair : GameInstance->ItemSlotsPerType)
	//{
	//	for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++)
	//	{
	//		SlottedItems.Add(FDAOItemSlot(Pair.Key, SlotNumber), nullptr);
	//	}
	//}

	UDAOSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	UDAOAssetManager& AssetManager = UDAOAssetManager::Get();
	if (CurrentSaveGame)
	{
		// Copy from save game into controller data
		bool bFoundAnySlots = false;
		/*for (const TPair<FPrimaryAssetId, FDAOItemInventoryData>& ItemPair : CurrentSaveGame->InventoryData)
		{
			UDAOItem* LoadedItem = AssetManager.ForceLoadItem(ItemPair.Key);

			if (LoadedItem != nullptr)
			{
				InventoryData.Add(LoadedItem, ItemPair.Value);
			}
		}*/

		// TODO Craig
	/*	for (const TPair<FDAOItemSlot, FPrimaryAssetId>& SlotPair : CurrentSaveGame->SlottedItems)
		{
			if (SlotPair.Value.IsValid())
			{
				UDAOItem* LoadedItem = AssetManager.ForceLoadItem(SlotPair.Value);
				if (GameInstance->IsValidItemSlot(SlotPair.Key) && LoadedItem)
				{
					SlottedItems.Add(SlotPair.Key, LoadedItem);
					bFoundAnySlots = true;
				}
			}
		}*/

		if (!bFoundAnySlots)
		{
			// Auto slot items as no slots were saved
			FillEmptySlots();
		}

		//NotifyInventoryLoaded();

		return true;
	}

	// Load failed but we reset inventory, so need to notify UI
	//NotifyInventoryLoaded();

	return false;
}

void ADAOPlayerController::HandleSaveGameLoaded(UDAOSaveGame* NewSaveGame)
{
	LoadInventory();
}

void ADAOPlayerController::FillEmptySlots()
{
	bool bShouldSave = false;
	//for (const TPair<UDAOItem*, FDAOItemInventoryData>& Pair : InventoryData)
	//{
	//	bShouldSave |= FillEmptySlotWithItem(Pair.Key);
	//}

	if (bShouldSave)
	{
		SaveInventory();
	}
}

void ADAOPlayerController::StartTargeting()
{
	// Change mouse cursor and lock
	SetMouseCursor(EMouseCursor::CardinalCross);
	bIsMouseCursorLocked = true;
	bSelectingTargetForCast = true;

	// Create Order Preview
	//UDAOOrderHelper::CreateOrderPreviewActor
}
