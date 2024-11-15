// Craig Duthie 2023

// This Include
#include "HealthBarComponent.h"

// Engine Includes
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

// Local Includes
#include "DAOBaseCharacter.h"
#include "HealthBarInterface.h"

// Called when the game starts
void UHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ADAOBaseCharacter* BaseCharacter = Cast<ADAOBaseCharacter>(GetOwner()))
	{
		BaseCharacterOwner = BaseCharacter;
		BaseCharacterOwner->OnHealthChanged.AddDynamic(this, &UHealthBarComponent::OnHealthChanged);

		if (UWidgetComponent* HealthBarComponent = BaseCharacterOwner->FindComponentByClass<UWidgetComponent>())
		{
			if (UUserWidget* HealthBarWidget = HealthBarComponent->GetWidget())
			{
				if (HealthBarWidget->GetClass()->ImplementsInterface(UHealthBarInterface::StaticClass()))
				{
					IHealthBarInterface::Execute_UpdateHealth(HealthBarWidget, BaseCharacterOwner->GetHealth(), BaseCharacterOwner->GetMaxHealth());
				}
			}
		}
	}	
}

void UHealthBarComponent::OnHealthChanged(float OldValue, float NewValue)
{
	// Get health bar widget 
	// Set the properties
	if (UWidgetComponent* HealthBarComponent = BaseCharacterOwner->FindComponentByClass<UWidgetComponent>())
	{
		if (UUserWidget* HealthBarWidget = HealthBarComponent->GetWidget())
		{
			if (HealthBarWidget->GetClass()->ImplementsInterface(UHealthBarInterface::StaticClass()))
			{
				IHealthBarInterface::Execute_UpdateHealth(HealthBarWidget, NewValue, BaseCharacterOwner->GetMaxHealth());
			}
		}
	}

	if (NewValue <= 0.0f)
	{
		// Hide the widget component
		if (UWidgetComponent* HealthBarComponent = BaseCharacterOwner->FindComponentByClass<UWidgetComponent>())
		{
			HealthBarComponent->SetVisibility(false);
		}
	}
}