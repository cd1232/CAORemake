// Craig Duthie 2024

#pragma once

#include "Containers/Set.h"
#include "Containers/UnrealString.h"
#include "Delegates/IDelegateInstance.h"
#include "GraphEditor.h"
#include "HAL/PlatformMath.h"
#include "Internationalization/Text.h"
#include "Layout/Visibility.h"
#include "Math/Color.h"
#include "Misc/NotifyHook.h"
#include "Templates/SharedPointer.h"
#include "Toolkits/IToolkit.h"
#include "Types/SlateEnums.h"
#include "UObject/NameTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "IKatsuraQuestTreeEditor.h"
#include "QuestTreeGraph.h"

class UKatsuraQuestTree;
class FDocumentTabFactory;
class FDocumentTracker;
class FObjectPostSaveContext;
class FProperty;
class IDetailsView;
class SWidget;
class UClass;
class UEdGraph;
class UEdGraphNode;
class UObject;
class UPackage;
struct FBlackboardEntry;
struct FPropertyChangedEvent;

/**
 * 
 */
class KATSURAQUESTEDITOR_API FKatsuraQuestTreeEditor : public IKatsuraQuestTreeEditor, public FEditorUndoClient, public FNotifyHook
{
public:
	FKatsuraQuestTreeEditor();
	virtual ~FKatsuraQuestTreeEditor();

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	bool IsPropertyEditable();
	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);
	void CreateInternalWidgets();
	virtual void InitQuestTreeEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* InObject);

	//~ Begin IToolkit Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	//~ End IToolkit Interface

	//~ Begin IKatsuraQuestTreeEditor Interface
	//virtual void InitializeDebuggerState(class FBehaviorTreeDebugger* ParentDebugger) const override;
	virtual UEdGraphNode* FindInjectedNode(int32 Index) const override;
	virtual void DoubleClickNode(class UEdGraphNode* Node) override;
	virtual void FocusAttentionOnNode(UEdGraphNode* Node) override;
	virtual void FocusWindow(UObject* ObjectToFocusOn = NULL) override;
	//~ End IKatsuraQuestTreeEditor Interface

	//~ Begin IAssetEditorInstance Interface
	virtual bool IncludeAssetInRestoreOpenAssetsPrompt(UObject* Asset) const override;
	//~ End IAssetEditorInstance Interface

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	// End of FEditorUndoClient

	//~ Begin FNotifyHook Interface
	virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged ) override;
	// End of FNotifyHook

	void OnPackageSaved(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);

	void JumpToNode(const UEdGraphNode* Node);
	
	/** Get the Quest tree we are editing (if any) */
	UKatsuraQuestTree* GetQuestTree() const;

	/** 
	 * Get the localized text to display for the specified mode 
	 * @param	InMode	The mode to display
	 * @return the localized text representation of the mode
	 */
	static FText GetLocalizedMode(FName InMode);

	/** Spawns the tab with the update graph inside */
	TSharedRef<SWidget> SpawnProperties();

	void RestoreQuestTree();

	void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor);

private:
	void OnSelectedNodesChanged(const TSet<UObject*>& Objects);
	
	// Delegates for graph editor commands
	void SelectAllNodes();
	bool CanSelectAllNodes() const;
	void DeleteSelectedNodes();
	bool CanDeleteNodes() const;
	void DeleteSelectedDuplicatableNodes();
	void CutSelectedNodes();
	bool CanCutNodes() const;
	void CopySelectedNodes();
	bool CanCopyNodes() const;
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes() const;
	void DuplicateNodes();
	bool CanDuplicateNodes() const;
	void OnCreateComment();

	void OnNodeDoubleClicked(UEdGraphNode* EdGraphNode);
	void OnNodeTitleCommitted(const FText& Text, ETextCommit::Type Arg, UEdGraphNode* EdGraphNode);

	/** Create widget for graph editing */
	FGraphPanelSelectionSet GetSelectedNodes() const;

	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);

	FGraphAppearanceInfo GetGraphAppearance() const;
	bool InEditingMode(bool bGraphIsEditable) const;

public:
	static const FName QuestTreeMode;
	static FText QuestTreeModeText;

protected:
	TSubclassOf<UQuestTreeGraph> GraphClass;
	FName GraphName;
	FText CornerText;
	FText TitleText;
	FText RootNodeNoteText;

private:
	TSharedPtr<FDocumentTracker> DocumentManager;
	TWeakPtr<FDocumentTabFactory> GraphEditorTabFactoryPtr;

	/** Property View */
	TSharedPtr<class IDetailsView> DetailsView;

	/** Currently focused graph */
	TWeakPtr<SGraphEditor> UpdateGraphEdPtr;
	
	/* The Quest Tree being edited */
	UKatsuraQuestTree* QuestTree;
	
	/** Command list for this editor */
	TSharedPtr<FUICommandList> GraphEditorCommands;

	/** Handle to the registered OnPackageSave delegate */
	FDelegateHandle OnPackageSavedDelegateHandle;
};
