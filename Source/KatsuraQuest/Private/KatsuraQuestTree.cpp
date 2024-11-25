// Craig Duthie 2024


#include "KatsuraQuestTree.h"

#if WITH_EDITOR
TSharedPtr<IQuestTreeGraphEditor> UKatsuraQuestTree::QuestTreeGraphEditor = nullptr;


void UKatsuraQuestTree::SetupQuestNode(UQuestNode* InQuestNode, bool bSelectNewNode)
{
	// Create the graph node
	check(InQuestNode->GraphNode == NULL);

	UKatsuraQuestTree::GetQuestTreeGraphEditor()->SetupQuestNode(QuestGraph, InQuestNode, bSelectNewNode);
}

void UKatsuraQuestTree::SetQuestTreeGraphEditor(TSharedPtr<IQuestTreeGraphEditor> InQuestTreeGraphEditor)
{
	check(!QuestTreeGraphEditor.IsValid());
	QuestTreeGraphEditor = InQuestTreeGraphEditor;
}

TSharedPtr<IQuestTreeGraphEditor> UKatsuraQuestTree::GetQuestTreeGraphEditor()
{
	return QuestTreeGraphEditor;	
}

UEdGraph* UKatsuraQuestTree::GetGraph() const
{
	return QuestGraph;
}
#endif // WITH_EDITOR
