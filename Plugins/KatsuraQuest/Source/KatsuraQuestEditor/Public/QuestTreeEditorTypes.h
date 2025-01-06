// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"

#include "QuestTreeEditorTypes.generated.h"

/**
 * 
 */
UCLASS()
class UQuestTreeEditorTypes : public UObject
{
	GENERATED_UCLASS_BODY()

	static const FName PinCategory_MultipleNodes;
	static const FName PinCategory_SingleComposite;
	static const FName PinCategory_SingleTask;
	static const FName PinCategory_SingleNode;
};
