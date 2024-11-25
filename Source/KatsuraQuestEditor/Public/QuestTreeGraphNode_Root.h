// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "QuestTreeGraphNode.h"
#include "QuestTreeGraphNode_Root.generated.h"

/**
 * 
 */
UCLASS()
class KATSURAQUESTEDITOR_API UQuestTreeGraphNode_Root : public UQuestTreeGraphNode
{
	GENERATED_BODY()

public:
	UQuestTreeGraphNode_Root(const FObjectInitializer& ObjectInitializer);
	
	virtual void PostPlacedNewNode() override;
	virtual void AllocateDefaultPins() override;
	virtual bool CanDuplicateNode() const override { return false; }
	virtual bool CanUserDeleteNode() const override{ return false; }
	//virtual bool HasErrors() const override { return false; }
	//virtual bool RefreshNodeClass() override { return false; }
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	/** gets icon resource name for title bar */
	//virtual FName GetNameIcon() const override;
	virtual FText GetTooltipText() const override;

	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
	//virtual FText GetDescription() const override;

	//virtual FLinearColor GetBackgroundColor(bool bIsActiveForDebugger) const override;
};
