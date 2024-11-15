// Craig Duthie 2023


// This Include
#include "SelectionComponent.h"

// Local Includes
#include "Core/DAOBaseCharacter.h"

void USelectionComponent::BeginPlay()
{
	Super::BeginPlay();

	SetSelected(bIsSelected);

	if (ADAOBaseCharacter* BaseCharacter = Cast<ADAOBaseCharacter>(GetOwner()))
	{
		BaseCharacterOwner = BaseCharacter;
		BaseCharacterOwner->OnHealthChanged.AddDynamic(this, &USelectionComponent::OnHealthChanged);
	}
}

void USelectionComponent::SetSelected(bool bInIsSelected)
{
	bIsSelected = bInIsSelected;
	SetVisibility(bIsSelected);
}

void USelectionComponent::OnHealthChanged(float OldValue, float NewValue)
{
	if (NewValue <= 0.0f)
	{
		SetSelected(false);
	}
}
