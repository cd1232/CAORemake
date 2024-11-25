// Craig Duthie 2024


#include "EdGraphSchema_QuestTree.h"

#include "AIGraphTypes.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "KatsuraQuestEditorModule.h"
#include "KatsuraQuestTree.h"
#include "KatsuraQuestTreeEditor.h"
#include "QuestNode.h"
#include "QuestTreeGraphNode.h"
#include "QuestTreeGraphNode_Root.h"
#include "Selection.h"

#define LOCTEXT_NAMESPACE "QuestTreeSchema"

UEdGraphNode* FQuestGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	check(QuestNodeClass);

	UKatsuraQuestTree* QuestTree = CastChecked<UQuestTreeGraph>(ParentGraph)->GetQuestTree();
	const FScopedTransaction Transaction( LOCTEXT("QuestTreeEditorNewQuestNode", "Quest Tree Editor: New Quest Node") );
	ParentGraph->Modify();
	QuestTree->Modify();

	UQuestNode* NewNode = QuestTree->ConstructQuestNode<UQuestNode>(QuestNodeClass, bSelectNewNode);

	// TODO
	// If this node allows >0 children but by default has zero - create a connector for starters
	// if (NewNode->GetMaxChildNodes() > 0 && NewNode->ChildNodes.Num() == 0)
	// {
	// 	NewNode->CreateStartingConnectors();
	// }

	// Attempt to connect inputs to selected nodes, unless we're already dragging from a single output
	if (FromPin == NULL || FromPin->Direction == EGPD_Input)
	{
		ConnectToSelectedNodes(NewNode, ParentGraph);
	}

	NewNode->GraphNode->NodePosX = Location.X;
	NewNode->GraphNode->NodePosY = Location.Y;

	NewNode->GraphNode->AutowireNewNode(FromPin);

	QuestTree->PostEditChange();
	QuestTree->MarkPackageDirty();

	return NewNode->GraphNode;
}

void FQuestGraphSchemaAction_NewNode::ConnectToSelectedNodes(UQuestNode* NewNodeClass, UEdGraph* ParentGraph) const
{
	//TODO
}

FGraphNodeClassHelper& UEdGraphSchema_QuestTree::GetClassCache() const
{
	const FKatsuraQuestEditorModule& EditorModule = FModuleManager::GetModuleChecked<FKatsuraQuestEditorModule>(TEXT("KatsuraQuestEditor"));
	FGraphNodeClassHelper* ClassHelper = EditorModule.GetClassCache().Get();
	check(ClassHelper);
	return *ClassHelper;
}

void UEdGraphSchema_QuestTree::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<UQuestTreeGraphNode> Creator(Graph);
	UQuestTreeGraphNode* MyNode = Creator.CreateNode(true, Cast<UQuestTreeGraph>(&Graph)->RootNodeClass);
	//MyNode->SetQuestNode(QuestNode);

	Creator.Finalize();
	SetNodeMetaData(MyNode, FNodeMetadata::DefaultGraphNode);
}

void UEdGraphSchema_QuestTree::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	FText SelectedItemText;

	FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();

	if (ContextMenuBuilder.FromPin == nullptr)
	{
		TArray<UQuestNode*> SelectedNodes;
		GEditor->GetSelectedObjects()->GetSelectedObjects<UQuestNode>(SelectedNodes);

		int32 TotalNodes = SelectedNodes.Num();

		if (TotalNodes > 1)
		{
			SelectedItemText = LOCTEXT("MultipleNodesSelected", "Multiple Nodes");
		}
		else if (TotalNodes == 1)
		{
			SelectedItemText = FText::FromString(SelectedNodes[0]->GetName());
		}
	}
	else
	{
		UQuestNode* SelectedNode = GEditor->GetSelectedObjects()->GetTop<UQuestNode>();
		if (SelectedNode && ContextMenuBuilder.FromPin->Direction == EGPD_Input)
		{
			SelectedItemText = FText::FromString(SelectedNode->GetName());
		}
	}

	bool bShowSelectedActions = !SelectedItemText.IsEmpty();


	TArray<UClass*> QuestNodeClasses;
	GetDerivedClasses(UQuestNode::StaticClass(), QuestNodeClasses, true);
	QuestNodeClasses.Sort();

	FGraphNodeClassHelper& ClassCache = GetClassCache();

	
	TArray<TSubclassOf<UQuestNode>> AllowedQuestNodes;

	TArray<FGraphNodeClassData> NodeClasses;
	ClassCache.GatherClasses(UQuestNode::StaticClass(), NodeClasses);
	
	for (FGraphNodeClassData QuestNodeClass : NodeClasses)
	{
		const UQuestNode* QuestNode = QuestNodeClass.GetClass()->GetDefaultObject<UQuestNode>();

		if (!ContextMenuBuilder.FromPin || ContextMenuBuilder.FromPin->Direction == EGPD_Input)// || QuestNode->GetMaxChildNodes() > 0)
		{
			const FText Name = FText::FromString(QuestNodeClass.GetClass()->GetDescription());

			{
				FFormatNamedArguments Arguments;
				Arguments.Add(TEXT("Name"), Name);
				const FText AddToolTip = FText::Format(LOCTEXT("NewQuestNodeTooltip", "Adds {Name} node here"), Arguments);
				TSharedPtr<FQuestGraphSchemaAction_NewNode> NewNodeAction(new FQuestGraphSchemaAction_NewNode(LOCTEXT("QuestNodeAction", "Quest Node"), Name, AddToolTip, 0));
				ContextMenuBuilder.AddAction(NewNodeAction);
				NewNodeAction->QuestNodeClass = QuestNodeClass.GetClass();
			}
		}
	}

	// TODO
	// Add the ability to create a comment to the context menu too for discoverability
	// {
	// 	TSharedPtr<FAISchemaAction_AddComment> Action = TSharedPtr<FAISchemaAction_AddComment>(
	// 		new FAISchemaAction_AddComment(LOCTEXT("AddComment", "Add Comment"), LOCTEXT("AddComment_Tooltip", "Adds a comment node to the graph."))
	// 	);
	//
	// 	ContextMenuBuilder.AddAction(Action);
	// }
}

#undef LOCTEXT_NAMESPACE