// Craig Duthie 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldCollision.h"

#include "InteractionComponent.generated.h"

class ADAOPlayerController;
class ADAOBaseCharacter;

// TODO Craig
//https://github.com/tranek/GASShooter/blob/master/Source/GASShooter/Public/Characters/Abilities/AbilityTasks/GSAT_WaitInteractableTarget.h

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DAO_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ProcessInteractable(AActor* Actor);

	ADAOBaseCharacter* GetSelectedCharacter() const;

protected:	
	UFUNCTION()
	void PerformTrace();

	void TraceDone(const FTraceHandle& TraceHandle, FTraceDatum& TraceData);

	UFUNCTION()
	void OnSelectedCharacterHealthChanged(float OldValue, float NewValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimerPeriod = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TEnumAsByte<ETraceTypeQuery> InteractionTraceType;

private:
	TWeakObjectPtr<ADAOPlayerController> PlayerController;
	
	/* If we have selected an enemy or conversation */
	TWeakObjectPtr<ADAOBaseCharacter> SelectedCharacter;

	FTimerHandle TraceTimerHandle;
	FTraceDelegate InteractionTraceDelegate;
};
