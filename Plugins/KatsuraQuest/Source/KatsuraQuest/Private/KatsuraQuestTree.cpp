// Craig Duthie 2024


#include "KatsuraQuestTree.h"


void UKatsuraQuestTree::InitializeInstance()
{
	TArray<UQuestNode*> OldAllNodes = AllNodes;
	AllNodes.Empty();
	
	for (UQuestNode* Node : OldAllNodes)
	{
		UQuestNode* NewNodeInstance = NewObject<UQuestNode>(this, Node->GetClass(), NAME_None, RF_Transient, Node, false, nullptr);
		AllNodes.Add(NewNodeInstance);
		NewNodeInstance->OnStateEntered.AddUObject(this, &UKatsuraQuestTree::OnNodeStateEntered);
		NewNodeInstance->OnStateExited.AddUObject(this, &UKatsuraQuestTree::OnNodeStateExited);

		if (Node == RootNode)
		{
			RootNode = NewNodeInstance;
		}
	}
}

bool UKatsuraQuestTree::SetState(EQuestState InState)
{
	if (QuestState != EQuestState::Completed)
	{
		QuestState = InState;
		OnStateUpdated.Broadcast(QuestState);
		return true;
	}

	return false;
}

EQuestState UKatsuraQuestTree::GetState() const
{
	if (QuestState == EQuestState::Completed ||
		QuestState == EQuestState::Unknown)
	{
		return QuestState;
	}

	if (QuestStateBotched)
	{
		return EQuestState::Botched;
	}

	return QuestState;
}

void UKatsuraQuestTree::BotchQuest()
{
	if (QuestState != EQuestState::Completed)
	{
		QuestStateBotched = true;
		OnStateUpdated.Broadcast(EQuestState::Botched);
	}
}

void UKatsuraQuestTree::UnbotchQuest()
{
	if (QuestState != EQuestState::Completed)
	{
		QuestStateBotched = false;
		OnStateUpdated.Broadcast(QuestState);
	}
}

TArray<UQuestNode*> UKatsuraQuestTree::GetCurrentActiveNodes() const
{
	return CurrentActiveNodes;
}

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

void UKatsuraQuestTree::CompileQuestNodesFromGraphNodes()
{
	UKatsuraQuestTree::GetQuestTreeGraphEditor()->CompileQuestNodesFromGraphNodes(this);
}

TSharedPtr<IQuestTreeGraphEditor> UKatsuraQuestTree::GetQuestTreeGraphEditor()
{
	return QuestTreeGraphEditor;	
}

UEdGraph* UKatsuraQuestTree::GetGraph() const
{
	return QuestGraph;
}

void UKatsuraQuestTree::PrintQuestTree()
{
		if (RootNode)
		{
			UE_LOG(LogTemp, Display, TEXT("RootNode: %s"), *RootNode->GetName());
			PrintChildren(RootNode);
		}
}

void UKatsuraQuestTree::PrintChildren(UQuestNode* Node)
{
	for (UQuestNode* ChildNode: Node->ChildNodes)
	{
		if (ChildNode)
		{
			UE_LOG(LogTemp, Display, TEXT("%s ChildNode: %s"), *Node->GetName(), *ChildNode->GetName());
			PrintChildren(ChildNode);
		}
	}
}

void UKatsuraQuestTree::OnNodeStateEntered(UQuestNode* QuestNode)
{
	CurrentActiveNodes.AddUnique(QuestNode);
}

void UKatsuraQuestTree::OnNodeStateExited(UQuestNode* QuestNode)
{
	CurrentActiveNodes.Remove(QuestNode);
}


#endif // WITH_EDITOR
