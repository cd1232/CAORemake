// Craig Duthie 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

UENUM(BlueprintType)
enum class EInteractableType : uint8
{
	None,
	Conversation,
	Enemy,
	Loot,
	Collectable
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DAO_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EInteractableType GetInteractableType() const { return IntertacableType; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EInteractableType IntertacableType = EInteractableType::None;		
};
