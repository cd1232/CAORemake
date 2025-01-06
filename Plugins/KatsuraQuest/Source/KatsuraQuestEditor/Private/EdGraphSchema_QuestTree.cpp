// Craig Duthie 2024


#include "EdGraphSchema_QuestTree.h"

#include "AIGraphTypes.h"
#include "GraphEditorActions.h"
#include "KatsuraQuestEditorModule.h"
#include "KatsuraQuestTree.h"
#include "KatsuraQuestTreeEditor.h"
#include "QuestNode.h"
#include "QuestTreeConnectionDrawingPolicy.h"
#include "QuestTreeGraphNode.h"
#include "QuestTreeGraphNode_Start.h"
#include "Selection.h"
#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "QuestTreeSchema"

int32 UEdGraphSchema_QuestTree::CurrentCacheRefreshID = 0;

UEdGraphNode* FQuestGraphSchemaAction_AddComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode_Comment* const CommentTemplate = NewObject<UEdGraphNode_Comment>();

	FVector2D SpawnLocation = Location;
	FSlateRect Bounds;

	TSharedPtr<SGraphEditor> GraphEditorPtr = SGraphEditor::FindGraphEditorForGraph(ParentGraph);
	if (GraphEditorPtr.IsValid())
	{
		// If they have a selection, build a bounding box around the selection
		if (GraphEditorPtr->GetBoundsForSelectedNodes(/*out*/ Bounds, 50.0f))
		{
			CommentTemplate->SetBounds(Bounds);
			SpawnLocation.X = CommentTemplate->NodePosX;
			SpawnLocation.Y = CommentTemplate->NodePosY;
		}
		else
		{
			// Otherwise initialize a default comment at the user's cursor location.
			SpawnLocation = GraphEditorPtr->GetPasteLocation();
		}
	}

	UEdGraphNode* const NewNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);

	return NewNode;
}

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
	{
		FGraphNodeCreator<UQuestTreeGraphNode> Creator(Graph);
		UQuestTreeGraphNode_Start* StartNode = Cast<UQuestTreeGraphNode_Start>(Creator.CreateNode(true, Cast<UQuestTreeGraph>(&Graph)->StartNodeClass));
		Creator.Finalize();
		SetNodeMetaData(StartNode, FNodeMetadata::DefaultGraphNode);
		StartNode->NodePosY = -100.0f;
	}

	{
		FGraphNodeCreator<UQuestTreeGraphNode> Creator(Graph);
		UQuestTreeGraphNode_Start* EndNode = Cast<UQuestTreeGraphNode_Start>(Creator.CreateNode(true, Cast<UQuestTreeGraph>(&Graph)->EndNodeClass));
		EndNode->bIsEndNode = true;
		Creator.Finalize();
		SetNodeMetaData(EndNode, FNodeMetadata::DefaultGraphNode);
		EndNode->NodePosY = 100.0f;
	}
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

		if (!ContextMenuBuilder.FromPin || ContextMenuBuilder.FromPin->Direction == EGPD_Output)// || QuestNode->GetMaxChildNodes() > 0)
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

	// Add the ability to create a comment to the context menu too for discoverability
	{
		TSharedPtr<FQuestGraphSchemaAction_AddComment> Action = TSharedPtr<FQuestGraphSchemaAction_AddComment>(
			new FQuestGraphSchemaAction_AddComment(LOCTEXT("AddComment", "Add Comment"), LOCTEXT("AddComment_Tooltip", "Adds a comment node to the graph."))
		);

		ContextMenuBuilder.AddAction(Action);
	}
}

FLinearColor UEdGraphSchema_QuestTree::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

bool UEdGraphSchema_QuestTree::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	check(Pin != NULL);
	return Pin->bDefaultValueIsIgnored;
}

FConnectionDrawingPolicy* UEdGraphSchema_QuestTree::CreateConnectionDrawingPolicy(int32 InBackLayerID,
	int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect,
	FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FQuestTreeConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

void UEdGraphSchema_QuestTree::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);

	CastChecked<UQuestTreeGraph>(TargetNode.GetGraph())->GetQuestTree()->CompileQuestNodesFromGraphNodes();
}

void UEdGraphSchema_QuestTree::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotification);

	// if this would notify the node then we need to compile the QuestTree
	if (bSendsNodeNotification)
	{
		CastChecked<UQuestTreeGraph>(TargetPin.GetOwningNode()->GetGraph())->GetQuestTree()->CompileQuestNodesFromGraphNodes();
	}
}

void UEdGraphSchema_QuestTree::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

TSharedPtr<FEdGraphSchemaAction> UEdGraphSchema_QuestTree::GetCreateCommentAction() const
{
	return TSharedPtr<FEdGraphSchemaAction>(static_cast<FEdGraphSchemaAction*>(new FQuestGraphSchemaAction_AddComment));
}

int32 UEdGraphSchema_QuestTree::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	if (Graph)
	{
		TSharedPtr<SGraphEditor> GraphEditorPtr = SGraphEditor::FindGraphEditorForGraph(Graph);
		if (GraphEditorPtr.IsValid())
		{
			return GraphEditorPtr->GetNumberOfSelectedNodes();
		}
	}

	return 0;
}

void UEdGraphSchema_QuestTree::GetContextMenuActions(class UToolMenu* Menu,
	UGraphNodeContextMenuContext* Context) const
{
	if (Context->Node && !Context->Pin)
	{
		const UQuestTreeGraphNode* QTGraphNode = Cast<const UQuestTreeGraphNode>(Context->Node);
		if (QTGraphNode && QTGraphNode->CanPlaceBreakpoints())
		{
			FToolMenuSection& Section = Menu->AddSection("EdGraphSchemaBreakpoints", LOCTEXT("BreakpointsHeader", "Breakpoints"));
			{
				Section.AddMenuEntry(FGraphEditorCommands::Get().ToggleBreakpoint);
				Section.AddMenuEntry(FGraphEditorCommands::Get().AddBreakpoint);
				Section.AddMenuEntry(FGraphEditorCommands::Get().RemoveBreakpoint);
				Section.AddMenuEntry(FGraphEditorCommands::Get().EnableBreakpoint);
				Section.AddMenuEntry(FGraphEditorCommands::Get().DisableBreakpoint);
			}
		}
	}
	
	if (Context->Node)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("BehaviorTreeGraphSchemaNodeActions", LOCTEXT("ClassActionsMenuHeader", "Node Actions"));
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGenericCommands::Get().Cut);
			Section.AddMenuEntry(FGenericCommands::Get().Copy);
			Section.AddMenuEntry(FGenericCommands::Get().Duplicate);

			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
		}
	}	

	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UEdGraphSchema_QuestTree::CanCreateConnection(const UEdGraphPin* PinA,
	const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionSameNode", "Both are on the same node"));
	}

	// Compare the directions
	const UEdGraphPin* InputPin = NULL;
	const UEdGraphPin* OutputPin = NULL;

	if (!CategorizePinsByDirection(PinA, PinB, /*out*/ InputPin, /*out*/ OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionIncompatible", "Directions are not compatible"));
	}

	class FNodeVisitorCycleChecker
	{
	public:
		/** Check whether a loop in the graph would be caused by linking the passed-in nodes */
		bool CheckForLoop(UEdGraphNode* StartNode, UEdGraphNode* EndNode)
		{
			VisitedNodes.Add(EndNode);
			return TraverseInputNodesToRoot(StartNode);
		}

	private:
		/** 
		 * Helper function for CheckForLoop()
		 * @param	Node	The node to start traversal at
		 * @return true if we reached a root node (i.e. a node with no input pins), false if we encounter a node we have already seen
		 */
		bool TraverseInputNodesToRoot(UEdGraphNode* Node)
		{
			VisitedNodes.Add(Node);

			// Follow every input pin until we cant any more ('root') or we reach a node we have seen (cycle)
			for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
			{
				UEdGraphPin* MyPin = Node->Pins[PinIndex];

				if (MyPin->Direction == EGPD_Input)
				{
					for (int32 LinkedPinIndex = 0; LinkedPinIndex < MyPin->LinkedTo.Num(); ++LinkedPinIndex)
					{
						UEdGraphPin* OtherPin = MyPin->LinkedTo[LinkedPinIndex];
						if( OtherPin )
						{
							UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
							if (VisitedNodes.Contains(OtherNode))
							{
								return false;
							}
							else
							{
								return TraverseInputNodesToRoot(OtherNode);
							}
						}
					}
				}
			}

			return true;
		}

		TSet<UEdGraphNode*> VisitedNodes;
	};

	// check for cycles
	FNodeVisitorCycleChecker CycleChecker;
	if(!CycleChecker.CheckForLoop(PinA->GetOwningNode(), PinB->GetOwningNode()))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorcycle", "Can't create a graph cycle"));
	}

	// Break existing connections on inputs only - multiple output connections are acceptable
	if (InputPin->LinkedTo.Num() > 0)
	{
		ECanCreateConnectionResponse ReplyBreakOutputs;
		if (InputPin == PinA)
		{
			ReplyBreakOutputs = CONNECT_RESPONSE_BREAK_OTHERS_A;
		}
		else
		{
			ReplyBreakOutputs = CONNECT_RESPONSE_BREAK_OTHERS_B;
		}
		return FPinConnectionResponse(ReplyBreakOutputs, LOCTEXT("ConnectionReplace", "Replace existing connections"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

bool UEdGraphSchema_QuestTree::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	bool bModified = UEdGraphSchema::TryCreateConnection(PinA, PinB);

	if (bModified)
	{
		CastChecked<UQuestTreeGraph>(PinA->GetOwningNode()->GetGraph())->GetQuestTree()->CompileQuestNodesFromGraphNodes();
	}

	return bModified;
}

const FPinConnectionResponse UEdGraphSchema_QuestTree::CanMergeNodes(const UEdGraphNode* A, const UEdGraphNode* B) const
{
	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT(""));
}

bool UEdGraphSchema_QuestTree::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UEdGraphSchema_QuestTree::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UEdGraphSchema_QuestTree::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}

#undef LOCTEXT_NAMESPACE
