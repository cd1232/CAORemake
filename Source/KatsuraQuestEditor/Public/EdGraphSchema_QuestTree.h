// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "AIGraphTypes.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_QuestTree.generated.h"

class UQuestNode;

/** Action to add a node to the graph */
USTRUCT()
struct KATSURAQUESTEDITOR_API FQuestGraphSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	/** Class of node we want to create */
	UPROPERTY()
	TSubclassOf<UQuestNode> QuestNodeClass;


	FQuestGraphSchemaAction_NewNode() 
		: FEdGraphSchemaAction()
		, QuestNodeClass(NULL)
	{}

	FQuestGraphSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
		, QuestNodeClass(NULL)
	{}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface

private:
	/** Connects new node to output of selected nodes */
	void ConnectToSelectedNodes(UQuestNode* NewNodeClass, UEdGraph* ParentGraph) const;
};

/**
 * 
 */
UCLASS()
class KATSURAQUESTEDITOR_API UEdGraphSchema_QuestTree : public UEdGraphSchema
{
	GENERATED_BODY()

	FGraphNodeClassHelper& GetClassCache() const;

	//~ Begin EdGraphSchema Interface
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	//~ End EdGraphSchema Interface

};


