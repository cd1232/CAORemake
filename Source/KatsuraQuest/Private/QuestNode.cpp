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
