// Craig Duthie 2024


#include "KatsuraQuestTreeEditor.h"

#include "EdGraphUtilities.h"
#include "GraphEditorActions.h"
#include "KatsuraQuestEditorModule.h"
#include "KatsuraQuestTree.h"
#include "QuestTreeColors.h"
#include "QuestTreeEditorApplicationMode.h"
#include "QuestTreeEditorTabFactories.h"
#include "QuestTreeGraphNode_Start.h"
#include "SNodePanel.h"
#include "Framework/Commands/GenericCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/ObjectSaveContext.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

#define LOCTEXT_NAMESPACE "QuestTreeEditor"

const FName FKatsuraQuestTreeEditor::QuestTreeMode(TEXT("QuestTree"));

FText FKatsuraQuestTreeEditor::QuestTreeModeText(LOCTEXT("QuestTreeMode", "Quest Tree"));


FKatsuraQuestTreeEditor::FKatsuraQuestTreeEditor()
	: IKatsuraQuestTreeEditor()
{
	// listen for package change events to update injected nodes
	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FKatsuraQuestTreeEditor::OnPackageSaved);
	
	QuestTree = nullptr;

	bCheckDirtyOnAssetSave = true;

	GraphClass = UQuestTreeGraph::StaticClass();
}

FKatsuraQuestTreeEditor::~FKatsuraQuestTreeEditor()
{
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);

	// TODO
	//Debugger.Reset();
}

void FKatsuraQuestTreeEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);

	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

bool FKatsuraQuestTreeEditor::IsPropertyEditable()
{
	// TODO
	// if (FBehaviorTreeDebugger::IsPIESimulating() || bForceDisablePropertyEdit)
	// {
	// 	return false;
	// }

	TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdPtr.Pin();
	return FocusedGraphEd.IsValid() && FocusedGraphEd->GetCurrentGraph() && FocusedGraphEd->GetCurrentGraph()->bEditable;
}

void FKatsuraQuestTreeEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	// 	if (FocusedGraphOwner.IsValid())
	// {
	// 	FocusedGraphOwner->OnInnerGraphChanged();
	// }
//
// 	// update abort range highlight when changing decorator's flow abort mode
// 	if (PropertyChangedEvent.Property)
// 	{
// 		if(PropertyChangedEvent.Property->GetFName() == TEXT("FlowAbortMode"))
// 		{
// 			bShowDecoratorRangeLower = false;
// 			bShowDecoratorRangeSelf = false;
//
// 			FGraphPanelSelectionSet CurrentSelection = GetSelectedNodes();
// 			if (CurrentSelection.Num() == 1)
// 			{
// 				for (FGraphPanelSelectionSet::TConstIterator It(CurrentSelection); It; ++It)
// 				{
// 					UBehaviorTreeGraphNode_Decorator* DecoratorNode = Cast<UBehaviorTreeGraphNode_Decorator>(*It);
// 					if (DecoratorNode)
// 					{
// 						FAbortDrawHelper Mode0, Mode1;
// 						GetAbortModePreview(Cast<UBTDecorator>(DecoratorNode->NodeInstance), Mode0, Mode1);
//
// 						UBehaviorTreeGraph* MyGraph = Cast<UBehaviorTreeGraph>(BehaviorTree->BTGraph);
// 						MyGraph->UpdateAbortHighlight(Mode0, Mode1);
// 					}
// 				}			
// 			}
// 		}
// 		else if(PropertyChangedEvent.Property->GetFName() == TEXT("BlackboardAsset"))
// 		{
// 			if(BlackboardView.IsValid())
// 			{
// 				BlackboardView->SetObject(GetBlackboardData());
// 			}
// 		}
// 	}
//
// 	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == TEXT("BehaviorAsset"))
// 	{
// 		UBehaviorTreeGraph* MyGraph = Cast<UBehaviorTreeGraph>(BehaviorTree->BTGraph);
// 		MyGraph->UpdateInjectedNodes();
// 		MyGraph->UpdateAsset(UBehaviorTreeGraph::ClearDebuggerFlags);
// 	}

	const TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdPtr.Pin();
	if (FocusedGraphEd.IsValid() && FocusedGraphEd->GetCurrentGraph())
	{
		FocusedGraphEd->GetCurrentGraph()->GetSchema()->ForceVisualizationCacheClear();
	}
}

void FKatsuraQuestTreeEditor::CreateInternalWidgets()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
	FDetailsViewArgs DetailsViewArgs; 
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.NotifyHook = this;
	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
	DetailsView = PropertyEditorModule.CreateDetailView( DetailsViewArgs );
	DetailsView->SetObject( NULL );
	DetailsView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateSP(this, &FKatsuraQuestTreeEditor::IsPropertyEditable));
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FKatsuraQuestTreeEditor::OnFinishedChangingProperties);
}

void FKatsuraQuestTreeEditor::InitQuestTreeEditor(const EToolkitMode::Type Mode,
                                                  const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* InObject)
{
	UKatsuraQuestTree* QuestTreeToEdit = Cast<UKatsuraQuestTree>(InObject);

	if (QuestTreeToEdit != nullptr)
	{
		QuestTree = QuestTreeToEdit;
	}

	TSharedPtr<FKatsuraQuestTreeEditor> ThisPtr(SharedThis(this));
	if (!DocumentManager.IsValid())
	{
		DocumentManager = MakeShareable(new FDocumentTracker);
		DocumentManager->Initialize(ThisPtr);
		
		{
			TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FQTGraphEditorSummoner(ThisPtr,
			FQTGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FKatsuraQuestTreeEditor::CreateGraphEditorWidget)
			));

			// Also store off a reference to the grapheditor factory so we can find all the tabs spawned by it later.
			GraphEditorTabFactoryPtr = GraphEditorFactory;
			DocumentManager->RegisterDocumentFactory(GraphEditorFactory);
		}
	}

	TArray<UObject*> ObjectsToEdit;
	if (QuestTree != nullptr)
	{
		ObjectsToEdit.Add(QuestTree);
	}

	// TODO
	// if (!ToolbarBuilder.IsValid())
	// {
	// 	ToolbarBuilder = MakeShareable(new FBehaviorTreeEditorToolbar(SharedThis(this)));
	// }

	const TArray<UObject*>* EditedObjects = GetObjectsCurrentlyBeingEdited();
	if (EditedObjects == nullptr || EditedObjects->Num() == 0)
	{
		FGraphEditorCommands::Register();
		// FBTCommonCommands::Register();
		// FBTDebuggerCommands::Register();

		const bool bCreateDefaultStandaloneMenu = true;
		const bool bCreateDefaultToolbar = true;
		InitAssetEditor( Mode, InitToolkitHost, FKatsuraQuestEditorModule::QuestTreeEditorAppIdentifier, FTabManager::FLayout::NullLayout, 
		bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectsToEdit );

		//BindCommonCommands();
		//ExtendMenu();
		CreateInternalWidgets();

		// TODO - Debugger
		// Debugger = MakeShareable(new FBehaviorTreeDebugger);
  //       Debugger->Setup(BehaviorTree, SharedThis(this));
  //       BindDebuggerToolbarCommands();

		//FKatsuraQuestEditorModule& QuestTreeEditorModule = FModuleManager::LoadModuleChecked<FKatsuraQuestEditorModule>("KatsuraQuestEditor");
		// AddMenuExtender(BehaviorTreeEditorModule.GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));
		// AddToolbarExtender(BehaviorTreeEditorModule.GetToolBarExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));
		
		AddApplicationMode(QuestTreeMode, MakeShareable(new FQuestTreeEditorApplicationMode(SharedThis(this))));
	}

	if (QuestTree != nullptr)
	{
		SetCurrentMode(QuestTreeMode);
	}
	
	// TODO
	//OnClassListUpdated();
	RegenerateMenusAndToolbars();
}

FName FKatsuraQuestTreeEditor::GetToolkitFName() const
{
	return FName("Quest Tree");
}

FText FKatsuraQuestTreeEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "QuestTree");
}

FString FKatsuraQuestTreeEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "QuestTree").ToString();
}

FLinearColor FKatsuraQuestTreeEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor( 0.0f, 0.0f, 0.2f, 0.5f );	
}

FText FKatsuraQuestTreeEditor::GetToolkitName() const
{
	if(QuestTree != nullptr)
	{
		return FAssetEditorToolkit::GetLabelForObject(QuestTree);
	}

	return FText();
}

FText FKatsuraQuestTreeEditor::GetToolkitToolTipText() const
{
	if(QuestTree != nullptr)
	{
		return FAssetEditorToolkit::GetToolTipTextForObject(QuestTree);
	}

	return FText();
}

UEdGraphNode* FKatsuraQuestTreeEditor::FindInjectedNode(int32 Index) const
{
	UQuestTreeGraph* QuestTreeGraph = Cast<UQuestTreeGraph>(QuestTree->QuestGraph);
	return QuestTreeGraph ? QuestTreeGraph->FindInjectedNode(Index) : nullptr;
}

void FKatsuraQuestTreeEditor::DoubleClickNode(class UEdGraphNode* Node)
{
	FocusAttentionOnNode(Node);
	// TODO
	//OnNodeDoubleClicked(Node);
}

void FKatsuraQuestTreeEditor::FocusAttentionOnNode(UEdGraphNode* Node)
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (CurrentGraphEditor.IsValid())
	{
		CurrentGraphEditor->ClearSelectionSet();
		CurrentGraphEditor->SetNodeSelection(Node, true);
	}
	JumpToNode(Node);
}

void FKatsuraQuestTreeEditor::FocusWindow(UObject* ObjectToFocusOn)
{
	FWorkflowCentricApplication::FocusWindow(ObjectToFocusOn);
}

bool FKatsuraQuestTreeEditor::IncludeAssetInRestoreOpenAssetsPrompt(UObject* Asset) const
{
	return Asset && !QuestTree;
}

void FKatsuraQuestTreeEditor::PostUndo(bool bSuccess)
{
	// TODO
	FEditorUndoClient::PostUndo(bSuccess);
}

void FKatsuraQuestTreeEditor::PostRedo(bool bSuccess)
{
	// TODO
	FEditorUndoClient::PostRedo(bSuccess);
}

void FKatsuraQuestTreeEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
	FProperty* PropertyThatChanged)
{
	if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		// TODO
		// if(PropertyChangedEvent.Property != nullptr && PropertyChangedEvent.Property->GetFName() == TEXT("BlackboardAsset"))
		// {
		// 	BlackboardData = BehaviorTree->BlackboardAsset;
		// }
		//
		// RefreshBlackboardViewsAssociatedObject();
	}
}

void FKatsuraQuestTreeEditor::OnPackageSaved(const FString& PackageFileName, UPackage* Package,
	FObjectPostSaveContext ObjectSaveContext)
{
	// TODO
	UQuestTreeGraph* MyGraph = QuestTree ? Cast<UQuestTreeGraph>(QuestTree->QuestGraph) : nullptr;
	if (MyGraph)
	{
		// const bool bUpdated = MyGraph->UpdateInjectedNodes();
		// if (bUpdated)
		// {
		// 	MyGraph->UpdateAsset(UBehaviorTreeGraph::ClearDebuggerFlags);
		// }
	}
}

void FKatsuraQuestTreeEditor::JumpToNode(const UEdGraphNode* Node)
{
	TSharedPtr<SDockTab> ActiveTab = DocumentManager->GetActiveTab();
	if (ActiveTab.IsValid())
	{
		TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(ActiveTab->GetContent());
		if (GraphEditor.IsValid())
		{
			GraphEditor->JumpToNode(Node, false);
		}
	}
}

UKatsuraQuestTree* FKatsuraQuestTreeEditor::GetQuestTree() const
{
	return QuestTree;
}

FText FKatsuraQuestTreeEditor::GetLocalizedMode(FName InMode)
{
	return QuestTreeModeText;
}

FGraphAppearanceInfo FKatsuraQuestTreeEditor::GetGraphAppearance() const
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = CornerText;

	// TODO
	// const int32 StepIdx = Debugger.IsValid() ? Debugger->GetShownStateIndex() : 0;
	// if (Debugger.IsValid() && !Debugger->IsDebuggerRunning())
	// {
	// 	AppearanceInfo.PIENotifyText = LOCTEXT("InactiveLabel", "INACTIVE");
	// }
	// else if (StepIdx)
	// {
	// 	AppearanceInfo.PIENotifyText = FText::Format(LOCTEXT("StepsBackLabelFmt", "STEPS BACK: {0}"), FText::AsNumber(StepIdx));
	// }
	// else if (FBehaviorTreeDebugger::IsPlaySessionPaused())
	// {
	// 	AppearanceInfo.PIENotifyText = LOCTEXT("PausedLabel", "PAUSED");
	// }
	//
	// if (Debugger.IsValid() && Debugger->IsBehaviorExecutionPaused())
	// {
	// 	AppearanceInfo.WarningText = LOCTEXT("BehaviorExecutionPausedLabel", "BEHAVIOR EXECUTION PAUSED");
	// }

	AppearanceInfo.PIENotifyText = LOCTEXT("InactiveLabel", "INACTIVE");
	
	return AppearanceInfo;
}

bool FKatsuraQuestTreeEditor::InEditingMode(bool bGraphIsEditable) const
{
	// TODO
	return bGraphIsEditable;// && FBehaviorTreeDebugger::IsPIENotSimulating();
}

TSharedRef<SWidget> FKatsuraQuestTreeEditor::SpawnProperties()
{
	return
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			DetailsView.ToSharedRef()
		];
}

void FKatsuraQuestTreeEditor::RestoreQuestTree()
{
	// Update BT asset data based on saved graph to have correct data in editor
	UQuestTreeGraph* MyGraph = Cast<UQuestTreeGraph>(QuestTree->QuestGraph);
	const bool bNewGraph = MyGraph == NULL;
	if (MyGraph == NULL)
	{
		const TSubclassOf<UEdGraphSchema> SchemaClass = GetDefault<UQuestTreeGraph>(GraphClass)->Schema;
		check(SchemaClass);
		QuestTree->QuestGraph = FBlueprintEditorUtils::CreateNewGraph(QuestTree, GraphName, GraphClass, SchemaClass);
		MyGraph = Cast<UQuestTreeGraph>(QuestTree->QuestGraph);

		// Initialize the behavior tree graph
		const UEdGraphSchema* Schema = MyGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*MyGraph);

		//MyGraph->OnCreated();
	}
	else
	{
		//MyGraph->OnLoaded();
	}

	//MyGraph->Initialize();

	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(MyGraph);
	TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, bNewGraph ? FDocumentTracker::OpenNewDocument : FDocumentTracker::RestorePreviousDocument);

	if(QuestTree->LastEditedDocuments.Num() > 0)
	{
		TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(DocumentTab->GetContent());
		GraphEditor->SetViewLocation(QuestTree->LastEditedDocuments[0].SavedViewOffset, QuestTree->LastEditedDocuments[0].SavedZoomAmount);
	}

	const bool bIncreaseVersionNum = false;
	if(bNewGraph)
	{
	//	MyGraph->UpdateAsset(UBehaviorTreeGraph::ClearDebuggerFlags | UBehaviorTreeGraph::KeepRebuildCounter);
		//RefreshBlackboardViewsAssociatedObject();
	}
	else
	{
	//	MyGraph->UpdateAsset(UBehaviorTreeGraph::KeepRebuildCounter);
	//	//RefreshDebugger();
	}

	// FAbortDrawHelper EmptyMode;
	// bShowDecoratorRangeLower = false;
	// bShowDecoratorRangeSelf = false;
	// bSelectedNodeIsInjected = false;
	// bSelectedNodeIsRootLevel = false;
	// MyGraph->UpdateAbortHighlight(EmptyMode, EmptyMode);
}

void FKatsuraQuestTreeEditor::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor)
{
	UpdateGraphEdPtr = InGraphEditor;

	FGraphPanelSelectionSet CurrentSelection;
	CurrentSelection = InGraphEditor->GetSelectedNodes();
	OnSelectedNodesChanged(CurrentSelection);
}

void FKatsuraQuestTreeEditor::OnSelectedNodesChanged(const TSet<UObject*>& NewSelection)
{
	//BehaviorTreeEditorUtils::FPropertySelectionInfo SelectionInfo;
	//TArray<UObject*> Selection = BehaviorTreeEditorUtils::GetSelectionForPropertyEditor(NewSelection, SelectionInfo);

	UQuestTreeGraph* MyGraph = Cast<UQuestTreeGraph>(QuestTree->QuestGraph);
	
	TArray<UObject*> Selection;
	if(NewSelection.Num())
	{
		for(TSet<class UObject*>::TConstIterator SetIt(NewSelection);SetIt;++SetIt)
		{
			if (Cast<UQuestTreeGraphNode_Start>(*SetIt))
			{
				Selection.Add(GetQuestTree());
			}
			else if (UQuestTreeGraphNode* GraphNode = Cast<UQuestTreeGraphNode>(*SetIt))
			{
				Selection.Add(GraphNode->QuestNode);
			}
			else
			{
				Selection.Add(*SetIt);
			}
		}
		//Selection = NewSelection.Array();
	}
	else
	{
		Selection.Add(GetQuestTree());
	}

	DetailsView->SetObjects(Selection);

	//MyGraph->UpdateAbortHighlight(Mode0, Mode1);
}

void FKatsuraQuestTreeEditor::SelectAllNodes()
{
	if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FKatsuraQuestTreeEditor::CanSelectAllNodes() const
{
	return true;
}

void FKatsuraQuestTreeEditor::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = CastChecked<UEdGraphNode>(*NodeIt);

		if (Node->CanUserDeleteNode())
		{
			if (UQuestTreeGraphNode* QuestTreeGraphNode = Cast<UQuestTreeGraphNode>(Node))
			{
				UQuestNode* DelNode = QuestTreeGraphNode->QuestNode;

				FBlueprintEditorUtils::RemoveNode(NULL, QuestTreeGraphNode, true);

				// Make sure QuestTree is updated to match graph
				QuestTree->CompileQuestNodesFromGraphNodes();

				// Remove this node from the Quest Tree's list of all QuestNodes
				QuestTree->AllNodes.Remove(DelNode);
				QuestTree->MarkPackageDirty();
			}
			else
			{
				FBlueprintEditorUtils::RemoveNode(NULL, Node, true);
			}
		}
	}
}

bool FKatsuraQuestTreeEditor::CanDeleteNodes() const
{
	// If any of the nodes can be deleted then we should allow deleting
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FKatsuraQuestTreeEditor::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FKatsuraQuestTreeEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FKatsuraQuestTreeEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FKatsuraQuestTreeEditor::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	TArray<UQuestTreeGraphNode*> SubNodes;

	FString ExportedText;

	int32 CopySubNodeIndex = 0;
	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		UQuestTreeGraphNode* AINode = Cast<UQuestTreeGraphNode>(Node);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		Node->PrepareForCopying();

		if (AINode)
		{
			// AINode->CopySubNodeIndex = CopySubNodeIndex;
			//
			// // append all subnodes for selection
			// for (int32 Idx = 0; Idx < AINode->SubNodes.Num(); Idx++)
			// {
			// 	AINode->SubNodes[Idx]->CopySubNodeIndex = CopySubNodeIndex;
			// 	SubNodes.Add(AINode->SubNodes[Idx]);
			// }
			//
			// CopySubNodeIndex++;
		}
	}

	for (int32 Idx = 0; Idx < SubNodes.Num(); Idx++)
	{
		SelectedNodes.Add(SubNodes[Idx]);
		SubNodes[Idx]->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UQuestTreeGraphNode* Node = Cast<UQuestTreeGraphNode>(*SelectedIter);
		if (Node)
		{
			Node->PostCopyNode();
		}
	}
}

bool FKatsuraQuestTreeEditor::CanCopyNodes() const
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void FKatsuraQuestTreeEditor::PasteNodes()
{
	if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}

void FKatsuraQuestTreeEditor::PasteNodesHere(const FVector2D& Location)
{
	// Undo/Redo support
	const FScopedTransaction Transaction( NSLOCTEXT("UnrealEd", "QuestTreeEditorPaste", "Paste Quest Tree Node") );
	QuestTree->GetGraph()->Modify();
	QuestTree->Modify();
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();

	
	// Clear the selection set (newly pasted stuff will be selected)
	CurrentGraphEditor->ClearSelectionSet();

	// Grab the text to paste from the clipboard.
	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	// Import the nodes
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(QuestTree->GetGraph(), TextToImport, /*out*/ PastedNodes);

	//Average position of nodes so we can move them while still maintaining relative distances to each other
	FVector2D AvgNodePosition(0.0f,0.0f);

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;
		AvgNodePosition.X += Node->NodePosX;
		AvgNodePosition.Y += Node->NodePosY;
	}

	if ( PastedNodes.Num() > 0 )
	{
		float InvNumNodes = 1.0f/float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;

		if (UQuestTreeGraphNode* QuestGraphNode = Cast<UQuestTreeGraphNode>(Node))
		{
			QuestTree->AllNodes.Add(QuestGraphNode->QuestNode);
		}

		// Select the newly pasted stuff
		CurrentGraphEditor->SetNodeSelection(Node, true);

		Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X ;
		Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y ;

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		// Give new node a different Guid from the old one
		Node->CreateNewGuid();
	}

	// Force new pasted QuestNodes to have same connections as graph nodes
	QuestTree->CompileQuestNodesFromGraphNodes();
	
	// Update UI
	CurrentGraphEditor->NotifyGraphChanged();

	QuestTree->PostEditChange();
	QuestTree->MarkPackageDirty();
}

bool FKatsuraQuestTreeEditor::CanPasteNodes() const
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FKatsuraQuestTreeEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FKatsuraQuestTreeEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FKatsuraQuestTreeEditor::OnCreateComment()
{
	
}

void FKatsuraQuestTreeEditor::OnNodeDoubleClicked(UEdGraphNode* EdGraphNode)
{
	UQuestTreeGraphNode* MyNode = Cast<UQuestTreeGraphNode>(EdGraphNode);
	// if (UBehaviorTreeGraphNode_CompositeDecorator* Decorator = Cast<UBehaviorTreeGraphNode_CompositeDecorator>(Node))
	// {
	// 	if (Decorator->GetBoundGraph())
	// 	{
	// 		TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(const_cast<UEdGraph*>(Decorator->GetBoundGraph()));
	// 		
	// 		TArray< TSharedPtr<SDockTab> > MatchingTabs;
	// 		DocumentManager->FindMatchingTabs(Payload, MatchingTabs);
	// 		if (MatchingTabs.Num())
	// 		{
	// 			DocumentManager->CloseTab(Payload);
	// 			DocumentManager->OpenDocument(Payload, FDocumentTracker::RestorePreviousDocument);
	// 		}
	// 		else
	// 		{
	// 			DocumentManager->OpenDocument(Payload, FDocumentTracker::OpenNewDocument);
	// 		}
	// 	}
	// }
	// else if (UBehaviorTreeGraphNode_Task* Task = Cast<UBehaviorTreeGraphNode_Task>(Node))
	// {
	// 	UBehaviorTree* SubTreeToOpen = nullptr;
	// 	if (UBTTask_RunBehavior* SubtreeTask = Cast<UBTTask_RunBehavior>(Task->NodeInstance))
	// 	{
	// 		SubTreeToOpen = SubtreeTask->GetSubtreeAsset();
	// 	}
	// 	else if (UBTTask_RunBehaviorDynamic* DynamicSubTreeTask = Cast<UBTTask_RunBehaviorDynamic>(Task->NodeInstance))
	// 	{
	// 		// if (Debugger)
	// 		// {
	// 		// 	SubTreeToOpen = Debugger->GetDynamicSubtreeTaskBehaviorTree(DynamicSubTreeTask);
	// 		// }
	// 		
	// 		if (!SubTreeToOpen)
	// 		{
	// 			SubTreeToOpen = DynamicSubTreeTask->GetDefaultBehaviorAsset();
	// 		}
	// 	}
	//
	// 	// if (SubTreeToOpen)
	// 	// {
	// 	// 	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(SubTreeToOpen);
	// 	//
	// 	// 	IQuestTreeGraphEditor* ChildNodeEditor = static_cast<IQuestTreeGraphEditor*>(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(SubTreeToOpen, true));
	// 	// 	if (ChildNodeEditor)
	// 	// 	{
	// 	// 		ChildNodeEditor->InitializeDebuggerState(Debugger.Get());
	// 	// 	}
	// 	// }
	// }

	if (MyNode && MyNode->QuestNode &&
		MyNode->QuestNode->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
	{
		UClass* NodeClass = MyNode->QuestNode->GetClass();
		UPackage* Pkg = NodeClass->GetOuterUPackage();
		FString ClassName = NodeClass->GetName().LeftChop(2);
		UBlueprint* BlueprintOb = FindObject<UBlueprint>(Pkg, *ClassName);
		if(BlueprintOb)
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(BlueprintOb);
		}
	}
}

void FKatsuraQuestTreeEditor::OnNodeTitleCommitted(const FText& Text, ETextCommit::Type Arg, UEdGraphNode* EdGraphNode)
{
	if (EdGraphNode)
	{
		static const FText TranscationTitle = FText::FromString(FString(TEXT("Rename Node")));
		const FScopedTransaction Transaction(TranscationTitle);
		EdGraphNode->Modify();
		EdGraphNode->OnRenameNode(Text.ToString());
	}
}

FGraphPanelSelectionSet FKatsuraQuestTreeEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdPtr.Pin())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

TSharedRef<SGraphEditor> FKatsuraQuestTreeEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph != NULL);

	if (!GraphEditorCommands.IsValid())
	{
		GraphEditorCommands = MakeShareable(new FUICommandList);

		
		GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
			FExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::SelectAllNodes),
			FCanExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::CanSelectAllNodes)
			);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
			FExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::DeleteSelectedNodes),
			FCanExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::CanDeleteNodes)
			);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
			FExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::CopySelectedNodes),
			FCanExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::CanCopyNodes)
		);
	
		GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
			FExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::CutSelectedNodes),
			FCanExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::CanCutNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
			FExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::PasteNodes),
			FCanExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::CanPasteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
			FExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::DuplicateNodes),
			FCanExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::CanDuplicateNodes)
			);

		GraphEditorCommands->MapAction(
			FGraphEditorCommands::Get().CreateComment,
			FExecuteAction::CreateRaw(this, &FKatsuraQuestTreeEditor::OnCreateComment)
		);

		
		// GraphEditorCommands->MapAction( FGraphEditorCommands::Get().RemoveExecutionPin,
		// 	FExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::OnRemoveInputPin ),
		// 	FCanExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::CanRemoveInputPin )
		// 	);
		//
		// GraphEditorCommands->MapAction( FGraphEditorCommands::Get().AddExecutionPin,
		// 	FExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::OnAddInputPin ),
		// 	FCanExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::CanAddInputPin )
		// 	);
		//
		// // Debug actions
		// GraphEditorCommands->MapAction( FGraphEditorCommands::Get().AddBreakpoint,
		// 	FExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::OnAddBreakpoint ),
		// 	FCanExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::CanAddBreakpoint ),
		// 	FIsActionChecked(),
		// 	FIsActionButtonVisible::CreateSP( this, &FKatsuraQuestTreeEditor::CanAddBreakpoint )
		// 	);
		//
		// GraphEditorCommands->MapAction( FGraphEditorCommands::Get().RemoveBreakpoint,
		// 	FExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::OnRemoveBreakpoint ),
		// 	FCanExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::CanRemoveBreakpoint ),
		// 	FIsActionChecked(),
		// 	FIsActionButtonVisible::CreateSP( this, &FKatsuraQuestTreeEditor::CanRemoveBreakpoint )
		// 	);
		//
		// GraphEditorCommands->MapAction( FGraphEditorCommands::Get().EnableBreakpoint,
		// 	FExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::OnEnableBreakpoint ),
		// 	FCanExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::CanEnableBreakpoint ),
		// 	FIsActionChecked(),
		// 	FIsActionButtonVisible::CreateSP( this, &FKatsuraQuestTreeEditor::CanEnableBreakpoint )
		// 	);
		//
		// GraphEditorCommands->MapAction( FGraphEditorCommands::Get().DisableBreakpoint,
		// 	FExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::OnDisableBreakpoint ),
		// 	FCanExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::CanDisableBreakpoint ),
		// 	FIsActionChecked(),
		// 	FIsActionButtonVisible::CreateSP( this, &FKatsuraQuestTreeEditor::CanDisableBreakpoint )
		// 	);
		//
		// GraphEditorCommands->MapAction( FGraphEditorCommands::Get().ToggleBreakpoint,
		// 	FExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::OnToggleBreakpoint ),
		// 	FCanExecuteAction::CreateSP( this, &FKatsuraQuestTreeEditor::CanToggleBreakpoint ),
		// 	FIsActionChecked(),
		// 	FIsActionButtonVisible::CreateSP( this, &FKatsuraQuestTreeEditor::CanToggleBreakpoint )
		// 	);
	}
	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FKatsuraQuestTreeEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FKatsuraQuestTreeEditor::OnNodeDoubleClicked);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FKatsuraQuestTreeEditor::OnNodeTitleCommitted);

	// Make title bar
	TSharedRef<SWidget> TitleBarWidget = 
		SNew(SBorder)
		.BorderImage( FAppStyle::GetBrush( TEXT("Graph.TitleBackground") ) )
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(TitleText)
				.TextStyle( FAppStyle::Get(), TEXT("GraphBreadcrumbButtonText") )
			]
		];

	// Make full graph editor
	const bool bGraphIsEditable = InGraph->bEditable;
	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(this, &FKatsuraQuestTreeEditor::InEditingMode, bGraphIsEditable)
		.Appearance(this, &FKatsuraQuestTreeEditor::GetGraphAppearance)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true);
}

#undef LOCTEXT_NAMESPACE
