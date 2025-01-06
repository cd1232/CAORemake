// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "QuestNode.h"
#include "QuestObjective.h"
#include "QuestNode_WithObjectives.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewObjectives, TArray<FQuestObjective>, QuestObjective);

USTRUCT(BlueprintType)
struct DAO_API FQuestObjectiveMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FQuestObjective> QuestObjectives;
	
};


/**
 * 
 */
UCLASS()
class DAO_API UQuestNode_WithObjectives : public UQuestNode
{
	GENERATED_BODY()

public:
	virtual void EnterState() override;

protected:
	UPROPERTY(BlueprintAssignable)
	FOnNewObjectives OnNewObjectives;
	
	UPROPERTY(EditAnywhere)
	TArray<FQuestObjective> QuestObjectives;
};
