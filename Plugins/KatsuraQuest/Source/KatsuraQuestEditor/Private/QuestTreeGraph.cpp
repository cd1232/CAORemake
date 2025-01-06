// Craig Duthie 2024


#include "QuestTreeGraph.h"

#include "EdGraphSchema_QuestTree.h"
#include "KatsuraQuestTree.h"
#include "QuestTreeColors.h"
#include "QuestTreeGraphNode.h"
#include "QuestTreeGraphNode_Start.h"
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

	virtual void CompileQuestNodesFromGraphNodes(UKatsuraQuestTree* QuestTree)
	{
		// Use GraphNodes to make QuestNode Connections
		TArray<UQuestNode*> ChildNodes;
		TArray<UEdGraphPin*> OutputPins;

		for (int32 NodeIndex = 0; NodeIndex < QuestTree->GetGraph()->Nodes.Num(); ++NodeIndex)
		{
			UQuestTreeGraphNode* GraphNode = Cast<UQuestTreeGraphNode>(QuestTree->GetGraph()->Nodes[NodeIndex]);
			if (GraphNode && GraphNode->QuestNode)
			{
				// Set ChildNodes of each QuestNode
				GraphNode->GetOutputPins(OutputPins);
				ChildNodes.Empty();
				for (int32 PinIndex = 0; PinIndex < OutputPins.Num(); ++PinIndex)
				{
					UEdGraphPin* ChildPin = OutputPins[PinIndex];

					if (ChildPin->LinkedTo.Num() > 0)
					{
						UQuestTreeGraphNode* GraphChildNode = CastChecked<UQuestTreeGraphNode>(ChildPin->LinkedTo[0]->GetOwningNode());
						ChildNodes.Add(GraphChildNode->QuestNode);
					}
				}

				GraphNode->QuestNode->SetFlags(RF_Transactional);
				GraphNode->QuestNode->Modify();
				GraphNode->QuestNode->SetChildNodes(ChildNodes);
				GraphNode->QuestNode->PostEditChange();
			}
			else
			{
				// Set FirstNode based on RootNode connection
				UQuestTreeGraphNode_Start* RootNode = Cast<UQuestTreeGraphNode_Start>(QuestTree->GetGraph()->Nodes[NodeIndex]);
				if (RootNode)
				{
					QuestTree->Modify();
					if (RootNode->Pins[0]->LinkedTo.Num() > 0)
					{
						QuestTree->RootNode = CastChecked<UQuestTreeGraphNode>(RootNode->Pins[0]->LinkedTo[0]->GetOwningNode())->QuestNode;
					}
					else
					{
						QuestTree->RootNode = nullptr;
					}
					QuestTree->PostEditChange();
				}
			}
		}
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
	StartNodeClass = UQuestTreeGraphNode_Start::StaticClass();
	EndNodeClass = UQuestTreeGraphNode_Start::StaticClass();
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
