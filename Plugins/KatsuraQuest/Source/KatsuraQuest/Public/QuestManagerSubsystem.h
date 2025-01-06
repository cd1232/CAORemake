// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuestManagerSubsystem.generated.h"

class UKatsuraQuestTree;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestUpdated, UKatsuraQuestTree*, Quest, EQuestState, NewState);

/**
 * 
 */
UCLASS()
class KATSURAQUEST_API UQuestManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	/** Starts quest tree. */
	UFUNCTION(BlueprintCallable, Category = "Quests")
    virtual bool StartQuest(UKatsuraQuestTree* QuestTree);

	void SaveQuests();
	void LoadQuests();
	
protected:
	UPROPERTY(BlueprintAssignable)
	FOnQuestUpdated OnQuestUpdated;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quests")
	TArray<UKatsuraQuestTree*> StartedQuests;
};
