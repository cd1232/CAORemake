// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class KATSURAQUESTEDITOR_API SGraphNode_QuestTree : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_QuestTree) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, class UQuestTreeGraphNode* InNode);

	virtual void CreatePinWidgets() override;
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

	FText GetDescription() const;

	FReply OnMouseDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
protected:
	TSharedPtr<SHorizontalBox> OutputPinBox;

	/** The node body widget, cached here so we can determine its size when we want ot position our overlays */
	TSharedPtr<SBorder> NodeBody;
	
private:
	UQuestTreeGraphNode* QuestNode;
};


class KATSURAQUESTEDITOR_API SQuestTreePin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SQuestTreePin){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	//~ Begin SGraphPin Interface
	virtual FSlateColor GetPinColor() const override;
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	//~ End SGraphPin Interface

	const FSlateBrush* GetPinBorder() const;
};
