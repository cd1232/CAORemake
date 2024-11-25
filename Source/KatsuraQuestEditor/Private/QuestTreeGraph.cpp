// Craig Duthie 2024


#include "QuestTreeGraph.h"

#include "EdGraphSchema_QuestTree.h"
#include "KatsuraQuestTree.h"
#include "QuestTreeGraphNode.h"
#include "QuestTreeGraphNode_Root.h"
#include "Kismet2/BlueprintEditorUtils.h"

class FQuestTreeGraphEditor : public IQuestTreeGraphEditor
{
public:
	FQuestTreeGraphEditor()
	{}

	~FQuestTreeGraphEditor()
	{}

	UEdGraph* CreateNewQuestTreeGraph(UKatsuraQuestTree* InQuestTree) override
	{
		UQuestTreeGraph* QuestTreeGraph = CastChecked<UQuestTreeGraph>(FBlueprintEditorUtils::CreateNewGraph(InQuestTree, NAME_None, UQuestTreeGraph::StaticClass(), UEdGraphSchema_QuestTree::StaticClass()));

		return QuestTreeGraph;
	}

	void SetupQuestNode(UEdGraph* QuestTreeGraph, UQuestNode* QuestNode, bool bSelectNewNode) override
	{
		FGraphNodeCreator<UQuestTreeGraphNode> NodeCreator(*QuestTreeGraph);
		UQuestTreeGraphNode* GraphNode = NodeCreator.CreateNode(bSelectNewNode);
		GraphNode->SetQuestNode(QuestNode);
		NodeCreator.Finalize();
	}
};


UQuestTreeGraph::UQuestTreeGraph(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	if (!UKatsuraQuestTree::GetQuestTreeGraphEditor().IsValid())
	{
		UKatsuraQuestTree::SetQuestTreeGraphEditor(TSharedPtr<IQuestTreeGraphEditor>(new FQuestTreeGraphEditor()));
	}
	
	Schema = UEdGraphSchema_QuestTree::StaticClass();
	RootNodeClass = UQuestTreeGraphNode_Root::StaticClass();
}

UKatsuraQuestTree* UQuestTreeGraph::GetQuestTree()
{
	return CastChecked<UKatsuraQuestTree>(GetOuter());
}

class UEdGraphNode* UQuestTreeGraph::FindInjectedNode(int32 Index)
{
	for (int32 NodeIdx = 0; NodeIdx < Nodes.Num(); NodeIdx++)
	{
		UQuestTreeGraphNode* MyNode = Cast<UQuestTreeGraphNode>(Nodes[NodeIdx]);
		if (MyNode && MyNode->IsRootNode())
		{
			return MyNode;
		}
	}

	return NULL;
}
