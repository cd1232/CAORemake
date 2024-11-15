// Craig Duthie 2023

// This Include
#include "InteractionComponent.h"

// Engine Includes
#include "Blueprint/WidgetBlueprintLibrary.h"

// Local Includes
#include "Player/DAOCharacter.h"
#include "InteractableComponent.h"
#include "Player/DAOPlayerController.h"
#include "Core/DAOBaseCharacter.h"
#include "SelectionComponent.h"
#include "Core/RelationshipComponent.h"
#include "Core/DAOGlobalTags.h"

UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ADAOPlayerController* PC = Cast<ADAOPlayerController>(GetOwner()))
	{
		PlayerController = PC;

		InteractionTraceDelegate.BindUObject(this, &UInteractionComponent::TraceDone);

		GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &UInteractionComponent::PerformTrace, TimerPeriod, true);
	}	
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void UInteractionComponent::ProcessInteractable(AActor* Actor)
{
	if (Actor)
	{
		if (UInteractableComponent* InteractableComponent = Actor->FindComponentByClass<UInteractableComponent>())
		{
			// Do we show the selection thing and change color depending on enemy/loot
			EInteractableType InteractableType = InteractableComponent->GetInteractableType();
			if (InteractableType == EInteractableType::Enemy)
			{
				if (ADAOBaseCharacter* EnemyCharacter = Cast<ADAOBaseCharacter>(Actor))
				{
					if (EnemyCharacter->IsAlive())
					{
						SelectedCharacter = EnemyCharacter;
						SelectedCharacter->OnHealthChanged.AddUniqueDynamic(this, &UInteractionComponent::OnSelectedCharacterHealthChanged);
					}
				}
			}
		}
	}
	else
	{
		// Reset the old selected character (if we have one) before resetting
		if (SelectedCharacter.IsValid())
		{
			if (USelectionComponent* SelectionComponent = SelectedCharacter.Get()->FindComponentByClass<USelectionComponent>())
			{
				SelectionComponent->SetSelected(false);
			}

			SelectedCharacter->OnHealthChanged.RemoveDynamic(this, &UInteractionComponent::OnSelectedCharacterHealthChanged);
		}

		SelectedCharacter.Reset();
	}

	// Update the selection
	if (SelectedCharacter.IsValid())
	{
		if (USelectionComponent* SelectionComponent = SelectedCharacter.Get()->FindComponentByClass<USelectionComponent>())
		{
			SelectionComponent->SetSelected(true);
		}
	}
}

ADAOBaseCharacter* UInteractionComponent::GetSelectedCharacter() const
{
	return SelectedCharacter.Get();
}

void UInteractionComponent::PerformTrace()
{
	float MousePositionX, MousePositionY;
	PlayerController->GetMousePosition(MousePositionX, MousePositionY);
	PlayerController->GetHitResultAtScreenPositionAsync(FVector2D(MousePositionX, MousePositionY), UEngineTypes::ConvertToCollisionChannel(InteractionTraceType), false, &InteractionTraceDelegate);
}

void UInteractionComponent::TraceDone(const FTraceHandle& TraceHandle, FTraceDatum& TraceData)
{
	if (TraceData.OutHits.Num() == 0)
	{
		PlayerController->SetMouseCursor(EMouseCursor::Default);
		return;
	}

	if (UInteractableComponent* InteractableComponent = TraceData.OutHits[0].GetActor()->FindComponentByClass<UInteractableComponent>())
	{
		EInteractableType InteractableType = InteractableComponent->GetInteractableType();
		switch (InteractableType)
		{
		case EInteractableType::None:
			PlayerController->SetMouseCursor(EMouseCursor::Default);
			break;
		case EInteractableType::Conversation:
			PlayerController->SetMouseCursor(EMouseCursor::ResizeLeftRight);
			break;
		case EInteractableType::Enemy:
			PlayerController->SetMouseCursor(EMouseCursor::TextEditBeam);
			break;
		case EInteractableType::Loot:
			PlayerController->SetMouseCursor(EMouseCursor::ResizeSouthEast);
			break;
		case EInteractableType::Collectable:
			PlayerController->SetMouseCursor(EMouseCursor::ResizeUpDown);
			break;
		default:
			break;
		}
	}
	else
	{
		PlayerController->SetMouseCursor(EMouseCursor::Default);
	}
}

void UInteractionComponent::OnSelectedCharacterHealthChanged(float OldValue, float NewValue)
{
	if (SelectedCharacter.IsValid())
	{
		if (!SelectedCharacter.Get()->IsAlive())
		{
			if (USelectionComponent* SelectionComponent = SelectedCharacter.Get()->FindComponentByClass<USelectionComponent>())
			{
				SelectionComponent->SetSelected(false);
			}
		}
	}
}
