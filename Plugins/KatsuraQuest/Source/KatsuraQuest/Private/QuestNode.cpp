// Craig Duthie 2024


#include "QuestNode.h"



#if WITH_EDITOR
void UQuestNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	MarkPackageDirty();
}

void UQuestNode::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UQuestNode* This = CastChecked<UQuestNode>(InThis);
	Collector.AddReferencedObject(This->GraphNode, This);

	Super::AddReferencedObjects(InThis, Collector);
}

void UQuestNode::SetChildNodes(TArray<UQuestNode*>& InChildNodes)
{
	ChildNodes = InChildNodes;	
}

void UQuestNode::EnterState()
{
	ReceiveEnterState();
}

void UQuestNode::ExitState()
{
	ReceiveExitState();

	// TODO
	// Should the quest node go to it's children and enter their state??
	// Should have a check to see if it can actually enter
	for (UQuestNode* ChildNode : ChildNodes)
	{
		ChildNode->EnterState();
	}
}

int32 UQuestNode::GetNodeID() const
{
	return NodeID;
}

void UQuestNode::PostLoad()
{
	Super::PostLoad();

	SetFlags(RF_Transactional);
}

#endif


#if WITH_EDITORONLY_DATA
UEdGraphNode* UQuestNode::GetGraphNode() const
{
	return GraphNode;
}
#endif
