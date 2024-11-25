// Craig Duthie 2024


#include "SGraphNode_QuestTree.h"

#include "QuestTreeColors.h"
#include "SlateOptMacros.h"
#include "QuestTreeGraphNode.h"
#include "Editor/KismetWidgets/Public/SLevelOfDetailBranchNode.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

void SGraphNode_QuestTree::Construct(const FArguments& InArgs, UQuestTreeGraphNode* InNode)
{
	this->GraphNode = InNode;
	this->QuestNode = InNode;

	this->UpdateGraphNode();
	
	/*
	ChildSlot
	[
		// Populate the widget
	];
	*/
}

void SGraphNode_QuestTree::CreatePinWidgets()
{
	UQuestTreeGraphNode* StateNode = CastChecked<UQuestTreeGraphNode>(GraphNode);

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SQuestTreePin, MyPin);
				//.ToolTipText( this, &SGraphNode_BehaviorTree::GetPinTooltip, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SGraphNode_QuestTree::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	
	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	OutputPinBox.Reset();
	
	if (QuestNode)
	{
		// Decorators
		// Services
	}
	
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<STextBlock> DescriptionText; 
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	TWeakPtr<SNodeTitle> WeakNodeTitle = NodeTitle;
	auto GetNodeTitlePlaceholderWidth = [WeakNodeTitle]() -> FOptionalSize
	{
		TSharedPtr<SNodeTitle> NodeTitlePin = WeakNodeTitle.Pin();
		const float DesiredWidth = (NodeTitlePin.IsValid()) ? NodeTitlePin->GetTitleSize().X : 0.0f;
		return FMath::Max(75.0f, DesiredWidth);
	};
	auto GetNodeTitlePlaceholderHeight = [WeakNodeTitle]() -> FOptionalSize
	{
		TSharedPtr<SNodeTitle> NodeTitlePin = WeakNodeTitle.Pin();
		const float DesiredHeight = (NodeTitlePin.IsValid()) ? NodeTitlePin->GetTitleSize().Y : 0.0f;
		return FMath::Max(22.0f, DesiredHeight);
	};

	const FMargin NodePadding = FMargin(8.0f);
	// TODO
	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );
	this->GetOrAddSlot( ENodeZone::Center )
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage( FAppStyle::GetBrush( "Graph.StateNode.Body" ) )
			.Padding(0.0f)
			.BorderBackgroundColor(QuestTreeColors::NodeBody::Default)
			//.BorderBackgroundColor( this, &SGraphNode_QuestTree::GetBorderBackgroundColor )
			.OnMouseButtonDown(this, &SGraphNode_QuestTree::OnMouseDown)
			[
				SNew(SOverlay)
				// Pins and node details
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SVerticalBox)

					// INPUT PIN AREA
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.MinDesiredHeight(NodePadding.Top)
						[
							SAssignNew(LeftNodeBox, SVerticalBox)
						]
					]

					// STATE NAME AREA
					+SVerticalBox::Slot()
					.Padding(FMargin(NodePadding.Left, 0.0f, NodePadding.Right, 0.0f))
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(NodeBody, SBorder)
							.BorderImage( FAppStyle::GetBrush("BTEditor.Graph.BTNode.Body") )
							.BorderBackgroundColor(QuestTreeColors::NodeBody::Default)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Center)
							.Visibility(EVisibility::SelfHitTestInvisible)
							[
								SNew(SOverlay)
								+SOverlay::Slot()
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Fill)
								[
									SNew(SVerticalBox)
									+SVerticalBox::Slot()
									.AutoHeight()
									[
										SNew(SHorizontalBox)										
										+SHorizontalBox::Slot()
										.AutoWidth()
										[
											SNew(SLevelOfDetailBranchNode)
											.UseLowDetailSlot(this, &SGraphNode_QuestTree::UseLowDetailNodeTitles)
											.LowDetail()
											[
												SNew(SBox)
												.WidthOverride_Lambda(GetNodeTitlePlaceholderWidth)
												.HeightOverride_Lambda(GetNodeTitlePlaceholderHeight)
											]
											.HighDetail()
											[
												SNew(SHorizontalBox)
												+SHorizontalBox::Slot()
												.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
												[
													SNew(SVerticalBox)
													+SVerticalBox::Slot()
													.AutoHeight()
													[
														SAssignNew(InlineEditableText, SInlineEditableTextBlock)
														.Style( FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText" )
														.Text( NodeTitle.Get(), &SNodeTitle::GetHeadTitle )
														.OnVerifyTextChanged(this, &SGraphNode_QuestTree::OnVerifyNameTextChanged)
														.OnTextCommitted(this, &SGraphNode_QuestTree::OnNameTextCommited)
														.IsReadOnly( this, &SGraphNode_QuestTree::IsNameReadOnly )
														.IsSelected(this, &SGraphNode_QuestTree::IsSelectedExclusively)
													]
													+SVerticalBox::Slot()
													.AutoHeight()
													[
														NodeTitle.ToSharedRef()
													]
												]
											]
										]
									]
									+SVerticalBox::Slot()
									.AutoHeight()
									[
										// DESCRIPTION MESSAGE
										SAssignNew(DescriptionText, STextBlock )
										//.Visibility(this, &SGraphNode_QuestTree::GetDescriptionVisibility)
										.Text(this, &SGraphNode_QuestTree::GetDescription)
									]
									// +SVerticalBox::Slot()
									// .AutoHeight()
									// [
									// 	SNew(STextBlock)
									// 	.Visibility(this, &SGraphNode_QuestTree::GetRuntimeDescriptionVisibility)
									// 	.Text(this, &SGraphNode_QuestTree::GetRuntimeDescription)
									// 	.TextStyle(FAppStyle::Get(), "SmallText")
									// ]
								]
								+SOverlay::Slot()
								.HAlign(HAlign_Right)
								.VAlign(VAlign_Fill)
								[
									SNew(SBorder)
									.BorderImage( FAppStyle::GetBrush("BTEditor.Graph.BTNode.Body") )
									.BorderBackgroundColor(QuestTreeColors::Debugger::SearchFailed)
									.Padding(FMargin(4.0f, 0.0f))
									//.Visibility(this, &SGraphNode_QuestTree::GetDebuggerSearchFailedMarkerVisibility)
								]
							]
						]
					]

					// OUTPUT PIN AREA
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.MinDesiredHeight(NodePadding.Bottom)
						[
							SAssignNew(RightNodeBox, SVerticalBox)
							+SVerticalBox::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							.Padding(5.0f,0.0f)
							.FillHeight(1.0f)
							[
								SAssignNew(OutputPinBox, SHorizontalBox)
							]
						]
					]
				]

				// // Drag marker overlay
				// +SOverlay::Slot()
				// .HAlign(HAlign_Fill)
				// .VAlign(VAlign_Top)
				// [
				// 	SNew(SBorder)
				// 	.BorderBackgroundColor(QuestTreeColors::Action::DragMarker)
				// 	.ColorAndOpacity(QuestTreeColors::Action::DragMarker)
				// 	.BorderImage(FAppStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
				// 	.Visibility(this, &SGraphNode_QuestTree::GetDragOverMarkerVisibility)
				// 	[
				// 		SNew(SBox)
				// 		.HeightOverride(4.f)
				// 	]
				// ]
				//
				// // Blueprint indicator overlay
				// +SOverlay::Slot()
				// .HAlign(HAlign_Left)
				// .VAlign(VAlign_Top)
				// [
				// 	SNew(SImage)
				// 	.Image(FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Blueprint")))
				// 	.Visibility(this, &SGraphNode_QuestTree::GetBlueprintIconVisibility)
				// ]
			]
		];
	
	CreatePinWidgets();
}

void SGraphNode_QuestTree::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner( SharedThis(this) );

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility( TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced) );
	}

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		LeftNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			.Padding(20.0f,0.0f)
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		// const bool bIsSingleTaskPin = PinObj && (PinObj->PinType.PinCategory == UBehaviorTreeEditorTypes::PinCategory_SingleTask);
		// if (bIsSingleTaskPin)
		// {
			OutputPinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(1.0f)
			[
				PinToAdd
			];
		// }
		// else
		// {
		// 	OutputPinBox->AddSlot()
		// 	.HAlign(HAlign_Fill)
		// 	.VAlign(VAlign_Fill)
		// 	.FillWidth(1.0f)
		// 	[
		// 		PinToAdd
		// 	];
		// }
		
		OutputPins.Add(PinToAdd);
	}
}

FText SGraphNode_QuestTree::GetDescription() const
{
	return QuestNode ? QuestNode->GetDescription() : FText::GetEmpty();
}

FReply SGraphNode_QuestTree::OnMouseDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	// if (TestNode && TestNode->IsSubNode())
	// {
	// 	GetOwnerPanel()->SelectionManager.ClickedOnNode(GraphNode, MouseEvent);
	// 	return FReply::Handled();
	// }

	return FReply::Unhandled();
}


void SQuestTreePin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SQuestTreePin::GetPinBorder)
		.BorderBackgroundColor(this, &SQuestTreePin::GetPinColor)
		.OnMouseButtonDown(this, &SQuestTreePin::OnPinMouseDown)
		.Cursor(this, &SQuestTreePin::GetPinCursor)
		.Padding(FMargin(10.0f))
		);
}

TSharedRef<SWidget>	SQuestTreePin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SQuestTreePin::GetPinBorder() const
{
	return FAppStyle::GetBrush(TEXT("Graph.StateNode.Body"));
}

FSlateColor SQuestTreePin::GetPinColor() const
{
	return FSlateColor(IsHovered() ? FLinearColor::Yellow : FLinearColor::Black);
}
