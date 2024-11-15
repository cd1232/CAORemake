// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMPlayerInfo.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API UVMPlayerInfo : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	void SetPlayerName(const FText& NewPlayerName)
	{
		UE_MVVM_SET_PROPERTY_VALUE(PlayerName, NewPlayerName);
	}

	FText GetPlayerName() const
	{
		return PlayerName;
	}

	void SetExperience(int32 NewExperience)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Experience, NewExperience);
	}

	int32 GetExperience() const
	{
		return Experience;
	}

	void SetExperienceToNextLevel(int32 NewExperienceToNextLevel)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ExperienceToNextLevel, NewExperienceToNextLevel);
	}	

	int32 GetExperienceToNextLevel() const
	{
		return ExperienceToNextLevel;
	}

	// Gets the percentage of experience left before level up
	UFUNCTION(BlueprintPure, FieldNotify)	 
	float GetExperiencePercent() const
	{
		if (ExperienceToNextLevel != 0)
		{
			return (float) Experience / (float) ExperienceToNextLevel;
		}
		else
			return 0;
	}

	void SetCurrentHealth(int32 NewHealth)
	{
		UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, NewHealth);
	}

	void SetMaxHealth(int32 NewMaxHealth)
	{
		UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, NewMaxHealth);
	}

	int32 GetCurrentHealth() const
	{
		return CurrentHealth;
	}

	int32 GetMaxHealth() const
	{
		return MaxHealth;
	}

	UFUNCTION(BlueprintPure, FieldNotify)	 
	float GetHealthPercent() const
	{
		if (MaxHealth != 0)
		{
			return (float)CurrentHealth / (float)MaxHealth;
		}
		else
			return 0;
	}

	void SetCurrentMana(int32 NewMana)
	{
		UE_MVVM_SET_PROPERTY_VALUE(CurrentMana, NewMana);
	}

	void SetMaxMana(int32 NewMaxMana)
	{
		UE_MVVM_SET_PROPERTY_VALUE(MaxMana, NewMaxMana);
	}

	int32 GetCurrentMana() const
	{
		return CurrentMana;
	}

	int32 GetMaxMana() const
	{
		return MaxMana;
	}

	UFUNCTION(BlueprintPure, FieldNotify)	 
	float GetManaPercent() const
	{
		if (MaxMana != 0)
		{
			return (float)CurrentMana / (float)MaxMana;
		}
		else
			return 0;
	}
	
protected:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FText PlayerName;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 CurrentHealth;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 MaxHealth;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 CurrentMana;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 MaxMana;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 Experience;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 ExperienceToNextLevel;
	
};
