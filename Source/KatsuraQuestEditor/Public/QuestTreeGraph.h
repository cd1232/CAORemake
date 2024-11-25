// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "HAL/Platform.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "EdGraph/EdGraph.h"

#include "QuestTreeGraph.generated.h"

class UQuestTreeGraphNode_Root;
class UKatsuraQuestTree;
class UEdGraphNode;
class UObject;

/**
 * 
 */
UCLASS()
class KATSURAQUESTEDITOR_API UQuestTreeGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	UQuestTreeGraph(const FObjectInitializer& ObjectInitializer);

	class UEdGraphNode* FindInjectedNode(int32 Index);

	UKatsuraQuestTree* GetQuestTree();
	
	// TODO BehaviorTreeGraph.h
	UPROPERTY()
	TSubclassOf<UQuestTreeGraphNode_Root> RootNodeClass;
};
