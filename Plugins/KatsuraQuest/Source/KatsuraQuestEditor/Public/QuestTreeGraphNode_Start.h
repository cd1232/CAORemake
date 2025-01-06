// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "QuestTreeGraphNode.h"
#include "QuestTreeGraphNode_Start.generated.h"

/**
 * 
 */
UCLASS()
class KATSURAQUESTEDITOR_API UQuestTreeGraphNode_Start : public UQuestTreeGraphNode
{
	GENERATED_BODY()

public:
	UQuestTreeGraphNode_Start(const FObjectInitializer& ObjectInitializer);
	
	virtual void AllocateDefaultPins() override;
	virtual bool CanDuplicateNode() const override { return false; }
	virtual bool CanUserDeleteNode() const override{ return false; }
	//virtual bool HasErrors() const override { return false; }
	//virtual bool RefreshNodeClass() override { return false; }
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanPlaceBreakpoints() const override { return false; }

public:
	bool bIsEndNode = false;
};
