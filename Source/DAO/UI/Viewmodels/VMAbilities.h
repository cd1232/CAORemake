// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "RTSGameplayAbility.h"

#include "VMAbilities.generated.h"

class URTSGameplayAbility;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FUIAbilityInfo
{
	GENERATED_BODY()

	FUIAbilityInfo()
	{}

	FUIAbilityInfo(const TSubclassOf<UGameplayAbility>& InAbility)
	{
		Ability = InAbility;
	}

public:
	UPROPERTY(BlueprintReadOnly)
	TSoftClassPtr<URTSGameplayAbility> Ability;
};

/**
 * 
 */
UCLASS()
class DAO_API UVMAbilities : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	void AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& hotbarAbilities)
	{
		AbilitiesArray.Reserve(hotbarAbilities.Num());
		for (int i = 0; i < hotbarAbilities.Num(); ++i)
		{
			AbilitiesArray.Push(FUIAbilityInfo(hotbarAbilities[i]));
		}
	
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilitiesArray);
	}

	UFUNCTION(BlueprintPure, FieldNotify)	 
	TArray<FUIAbilityInfo> GetAbilitiesArray() const
	{
		return AbilitiesArray;
	}
	
protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FUIAbilityInfo> AbilitiesArray;
};