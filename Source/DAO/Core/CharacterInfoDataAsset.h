// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CharacterInfoDataAsset.generated.h"

class ULyraInventoryItemDefinition;

USTRUCT(BlueprintType)
struct FDefaultInventoryItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<ULyraInventoryItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsEquipped = false;
};


/**
 * 
 */
UCLASS()
class DAO_API UCharacterInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag CharacterTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UAnimInstance> AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> CharacterIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDefaultInventoryItem> DefaultInventory;	
};
