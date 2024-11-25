// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterInfoDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API UCharacterInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, blueprintReadOnly)
	TSoftClassPtr<UAnimInstance> AnimInstance;

	UPROPERTY(EditAnywhere, blueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	
	UPROPERTY(EditAnywhere, blueprintReadOnly)
	TObjectPtr<UTexture2D> CharacterIcon;
};
