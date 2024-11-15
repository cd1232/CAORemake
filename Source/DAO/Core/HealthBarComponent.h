// Craig Duthie 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HealthBarComponent.generated.h"

class ADAOBaseCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DAO_API UHealthBarComponent : public USceneComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHealthChanged(float OldValue, float NewValue);

protected:
	TWeakObjectPtr<ADAOBaseCharacter> BaseCharacterOwner;		
};
