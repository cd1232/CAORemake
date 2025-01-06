// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/TriggerVolume.h"
#include "Trigger_QuestOverlap.generated.h"

UCLASS()
class DAO_API ATrigger_QuestOverlap : public ATriggerVolume
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrigger_QuestOverlap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag TagToBroadcast;
	
	UPROPERTY(EditAnywhere)
	bool bDisableOverlapAfterTrigger = true;
};
