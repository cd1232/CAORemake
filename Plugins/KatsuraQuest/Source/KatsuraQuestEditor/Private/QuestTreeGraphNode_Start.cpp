// Craig Duthie 2024


#include "QuestTreeGraphNode_Start.h"

UQuestTreeGraphNode_Start::UQuestTreeGraphNode_Start(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bIsReadOnly = true;
}

void UQuestTreeGraphNode_Start::AllocateDefaultPins()
{
	if (bIsEndNode)
	{
		CreatePin(EGPD_Input, TEXT("QuestNode"), TEXT("Input"));
	}
	else
	{
		CreatePin(EGPD_Output, TEXT("QuestNode"), TEXT("Output"));
	}
}

FText UQuestTreeGraphNode_Start::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("QuestTreeEditor", "StartNode", "Start Node");
}