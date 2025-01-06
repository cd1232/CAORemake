// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "QuestNode.h"
#include "QuestNode_WithObjectives.h"
#include "Messages/EmptyMessageStruct.h"

#include "QuestNode_WaitForOverlap.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API UQuestNode_WaitForOverlap : public UQuestNode_WithObjectives
{
	GENERATED_BODY()
	
public:
	virtual void EnterState() override;
	virtual void ExitState() override;

protected:
	void OnOverlap(FGameplayTag BroadcastedTag, const FEmptyMessageStruct& Payload);

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag TagToListenFor;
};
