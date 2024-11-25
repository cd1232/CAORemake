// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestNode.generated.h"

/**
 * 
 */
UCLASS()
class KATSURAQUEST_API UQuestNode : public UObject
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
#endif //WITH_EDITOR

	//EnterState
	//TickState?
	//ExitState
	//GameplayTagsToSet
	
public:
	UPROPERTY()
	TArray<TObjectPtr<class UQuestNode>> ChildNodes;

#if WITH_EDITORONLY_DATA
	/** Node's Graph representation, used to get position. */
	UPROPERTY()
	TObjectPtr<UEdGraphNode>	GraphNode;

	class UEdGraphNode* GetGraphNode() const;
#endif
};
