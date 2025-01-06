// Craig Duthie 2024

#include "QuestObjective.generated.h"

#pragma once

USTRUCT(BlueprintType)
struct FQuestObjective
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
};
