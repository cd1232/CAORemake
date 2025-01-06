#include "KatsuraQuestEditorModule.h"

#include "EdGraphUtilities.h"
#include "KatsuraQuestTreeEditor.h"
#include "QuestNode.h"
#include "QuestTreeGraphNode.h"
#include "SGraphNode_QuestTree.h"

#define LOCTEXT_NAMESPACE "FKatsuraQuestEditorModule"

const FName FKatsuraQuestEditorModule::QuestTreeEditorAppIdentifier(TEXT("QuestTreeEditorApp"));

class FGraphPanelNodeFactory_QuestTree : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (UQuestTreeGraphNode* QuestTreeGraphNode = Cast<UQuestTreeGraphNode>(Node))
		{
			return SNew(SGraphNode_QuestTree, QuestTreeGraphNode);
		}

		return nullptr;
	}
};

TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_QuestTree;

void FKatsuraQuestEditorModule::StartupModule()
{
	GraphPanelNodeFactory_QuestTree = MakeShareable(new FGraphPanelNodeFactory_QuestTree());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_QuestTree);
}

void FKatsuraQuestEditorModule::ShutdownModule()
{
	if (!UObjectInitialized())
	{
		return;
	}

	ClassCache.Reset();
	
	if (GraphPanelNodeFactory_QuestTree.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_QuestTree);
		GraphPanelNodeFactory_QuestTree.Reset();
	}
}

TSharedRef<IKatsuraQuestTreeEditor> FKatsuraQuestEditorModule::CreateQuestTreeEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object)
{
	if (!ClassCache.IsValid())
	{
		ClassCache = MakeShareable(new FGraphNodeClassHelper(UQuestNode::StaticClass()));
		ClassCache->UpdateAvailableBlueprintClasses();
	}
	
	TSharedRef<FKatsuraQuestTreeEditor> NewQuestTreeEditor(new FKatsuraQuestTreeEditor());
	NewQuestTreeEditor->InitQuestTreeEditor(Mode, InitToolkitHost, Object);
	return NewQuestTreeEditor;
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FKatsuraQuestEditorModule, KatsuraQuestEditor)