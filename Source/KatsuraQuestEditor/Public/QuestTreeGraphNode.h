// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "QuestTreeGraphNode.generated.h"

class UQuestNode;
/**
 * 
 */
UCLASS()
class KATSURAQUESTEDITOR_API UQuestTreeGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UQuestTreeGraphNode(const FObjectInitializer& ObjectInitializer);
	
	/** Create all of the input pins required */
	virtual void CreateInputPins() {};
	/** Is this the undeletable root node */
	virtual bool IsRootNode() const {return false;}

	void SetQuestNode(UQuestNode* InQuestNode);

	FText GetDescription() const;

	/** if set, all modifications (including delete/cut) are disabled */
	UPROPERTY()
	uint32 bIsReadOnly : 1;
	
	/** The QuestNode this represents */
	UPROPERTY(VisibleAnywhere, instanced, Category=Quest)
	TObjectPtr<UQuestNode> QuestNode;
	
	
	//
	// /** Get the Output Pin (should only ever be one) */
	// class UEdGraphPin* GetOutputPin();
	// /** Get all of the Input Pins */
	// void GetInputPins(TArray<class UEdGraphPin*>& OutInputPins);
	// /** Get a single Input Pin via its index */
	// class UEdGraphPin* GetInputPin(int32 InputIndex);
	// /** Get the current Input Pin count */
	// int32 GetInputCount() const;
	//
	// /**
	//  * Handles inserting the node between the FromPin and what the FromPin was original connected to
	//  *
	//  * @param FromPin			The pin this node is being spawned from
	//  * @param NewLinkPin		The new pin the FromPin will connect to
	//  * @param OutNodeList		Any nodes that are modified will get added to this list for notification purposes
	//  */
	// void InsertNewNode(UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList);
	//
	// // UEdGraphNode interface.
	// virtual void AllocateDefaultPins() override;
	// virtual void ReconstructNode() override;
	// virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	// virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	// virtual FString GetDocumentationLink() const override;
	// End of UEdGraphNode interface.
};
