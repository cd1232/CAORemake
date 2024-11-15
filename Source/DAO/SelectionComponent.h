// Craig Duthie 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/DecalComponent.h"
#include "SelectionComponent.generated.h"

class ADAOBaseCharacter;

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class DAO_API USelectionComponent : public UDecalComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void SetSelected(bool bInIsSelected);

	bool IsSelected() const { return bIsSelected; }

private:
	UFUNCTION()
	void OnHealthChanged(float OldValue, float NewValue);

private:
	bool bIsSelected = false;	

	UPROPERTY()
	ADAOBaseCharacter* BaseCharacterOwner;
};
