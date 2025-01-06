// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "QuestNode.generated.h"

class UQuestNode;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateChanged, UQuestNode*);

/**
 * 
 */
UCLASS()
class KATSURAQUEST_API UQuestNode : public UObject
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
	
	/**
	 * Set the entire Child Node array directly, allows GraphNodes to fully control node layout.
	 * Can be overwritten to set up additional parameters that are tied to children.
	 */
	 virtual void SetChildNodes(TArray<UQuestNode*>& InChildNodes);
#endif //WITH_EDITOR

	virtual void EnterState();
	virtual void ExitState();

	int32 GetNodeID() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "EnterState"))
	void ReceiveEnterState();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "ExitState"))
	void ReceiveExitState();
	
	//TickState?
	
public:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UQuestNode>> ChildNodes;
	
	FOnStateChanged OnStateEntered;
	FOnStateChanged OnStateExited;
	
protected:
	UPROPERTY(VisibleAnywhere)
	int32 NodeID = INDEX_NONE;

public:
#if WITH_EDITORONLY_DATA
	/** Node's Graph representation, used to get position. */
	UPROPERTY()
	TObjectPtr<UEdGraphNode>	GraphNode;

	UEdGraphNode* GetGraphNode() const;
#endif
};
