// Craig Duthie 2024


#include "QuestTreeGraphNode_Root.h"

UQuestTreeGraphNode_Root::UQuestTreeGraphNode_Root(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bIsReadOnly = true;
}

void UQuestTreeGraphNode_Root::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
}

void UQuestTreeGraphNode_Root::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, TEXT("QuestNode"), TEXT("Output"));
}

FText UQuestTreeGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("QuestTreeEditor", "Root", "ROOT");
}

FText UQuestTreeGraphNode_Root::GetTooltipText() const
{
	return UEdGraphNode::GetTooltipText();
}

void UQuestTreeGraphNode_Root::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UQuestTreeGraphNode_Root::PostEditUndo()
{
	Super::PostEditUndo();
}
