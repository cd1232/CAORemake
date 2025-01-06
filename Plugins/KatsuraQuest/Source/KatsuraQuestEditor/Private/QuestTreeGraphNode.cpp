// Craig Duthie 2024


#include "QuestTreeGraphNode.h"
#include "QuestNode.h"
#include "QuestTreeEditorTypes.h"
#include "QuestTreeGraph.h"
#include "KatsuraQuestTree.h"

UQuestTreeGraphNode::UQuestTreeGraphNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bIsReadOnly = false;
}

void UQuestTreeGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UQuestTreeEditorTypes::PinCategory_MultipleNodes, TEXT("In"));
	CreatePin(EGPD_Output, UQuestTreeEditorTypes::PinCategory_MultipleNodes, TEXT("Out"));
}

void UQuestTreeGraphNode::PrepareForCopying()
{
	if (QuestNode)
	{
		// Temporarily take ownership of the QuestNode, so that it is not deleted when cutting
		QuestNode->Rename(NULL, this, REN_DontCreateRedirectors);
	}
}

void UQuestTreeGraphNode::GetInputPins(TArray<class UEdGraphPin*>& OutInputPins)
{
	OutInputPins.Empty();

	for (int32 PinIndex = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Input)
		{
			OutInputPins.Add(Pins[PinIndex]);
		}
	}
}

void UQuestTreeGraphNode::GetOutputPins(TArray<class UEdGraphPin*>& OutOutputPins)
{
	OutOutputPins.Empty();

	for (int32 PinIndex = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Output)
		{
			OutOutputPins.Add(Pins[PinIndex]);
		}
	}
}

void UQuestTreeGraphNode::SetQuestNode(UQuestNode* InQuestNode)
{
	QuestNode = InQuestNode;
	InQuestNode->GraphNode = this;
}

void UQuestTreeGraphNode::PostCopyNode()
{
	ResetNodeOwner();
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

int32 UQuestTreeGraphNode::GetNodeID() const
{
	if (QuestNode == nullptr)
	{
		return INDEX_NONE;
	}

	return QuestNode->GetNodeID();
}

void UQuestTreeGraphNode::PostEditImport()
{
	ResetNodeOwner();
}

void UQuestTreeGraphNode::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	if (!bDuplicateForPIE)
	{
		CreateNewGuid();
	}
}

void UQuestTreeGraphNode::PostLoad()
{
	Super::PostLoad();

	// Fixup any QuestNode back pointers that may be out of date
	if (QuestNode)
	{
		QuestNode->GraphNode = this;
	}

	for (int32 Index = 0; Index < Pins.Num(); ++Index)
	{
		UEdGraphPin* Pin = Pins[Index];
		if (Pin->PinName.IsNone())
		{
			// Makes sure pin has a name for lookup purposes but user will never see it
			if (Pin->Direction == EGPD_Input)
			{
				Pin->PinName = CreateUniquePinName(TEXT("Input"));
			}
			else
			{
				Pin->PinName = CreateUniquePinName(TEXT("Output"));
			}
			Pin->PinFriendlyName = FText::FromString(TEXT(" "));
		}
	}
}

void UQuestTreeGraphNode::ResetNodeOwner()
{
	if (QuestNode)
	{
		UKatsuraQuestTree* QuestTree = CastChecked<UQuestTreeGraph>(GetGraph())->GetQuestTree();
		
		if (QuestNode->GetOuter() != QuestTree)
		{
			// Ensures QuestNode is owned by the QuestTree
			QuestNode->Rename(NULL, QuestTree, REN_DontCreateRedirectors);
		}

		// Set up the back pointer for newly created quest nodes
		QuestNode->GraphNode = this;
	}
}
