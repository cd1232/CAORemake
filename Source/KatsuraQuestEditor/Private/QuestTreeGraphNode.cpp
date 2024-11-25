// Craig Duthie 2024


#include "QuestTreeGraphNode.h"
#include "QuestNode.h"

UQuestTreeGraphNode::UQuestTreeGraphNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bIsReadOnly = false;
}

void UQuestTreeGraphNode::SetQuestNode(UQuestNode* InQuestNode)
{
	QuestNode = InQuestNode;
	InQuestNode->GraphNode = this;
}

FText UQuestTreeGraphNode::GetDescription() const
{
	FString StringDesc;
	if (QuestNode == nullptr || QuestNode->GetClass() == nullptr)
	{
		StringDesc = TEXT("None");
		return FText::FromString(StringDesc);
	}

	if (QuestNode->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
	{
		StringDesc = QuestNode->GetClass()->GetName().LeftChop(2);
	}

	FString TypeDesc = QuestNode->GetClass()->GetName();
	const int32 ShortNameIdx = TypeDesc.Find(TEXT("_"), ESearchCase::CaseSensitive);
	if (ShortNameIdx != INDEX_NONE)
	{
		TypeDesc.MidInline(ShortNameIdx + 1, MAX_int32, EAllowShrinking::No);
	}

	StringDesc = TypeDesc;
	return FText::FromString(StringDesc);
}
