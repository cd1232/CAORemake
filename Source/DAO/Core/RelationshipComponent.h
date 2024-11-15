// Craig Duthie 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "RelationshipComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DAO_API URelationshipComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URelationshipComponent();

	const FGameplayTag& GetRelationshipTag() const { return RelationshipTag; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="Relationship"))
	FGameplayTag RelationshipTag;
};
