// Craig Duthie 2024


#include "QuestTreeConnectionDrawingPolicy.h"

#include "QuestTreeColors.h"
#include "QuestTreeGraphNode.h"
#include "Containers/EnumAsByte.h"
#include "Containers/Set.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Layout/ArrangedChildren.h"
#include "Layout/ArrangedWidget.h"
#include "Layout/PaintGeometry.h"
#include "Math/UnrealMathSSE.h"
#include "Misc/Optional.h"
#include "Rendering/DrawElements.h"
#include "Rendering/RenderingCommon.h"
#include "SGraphNode.h"
#include "Styling/SlateBrush.h"


FQuestTreeConnectionDrawingPolicy::FQuestTreeConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
	float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
		, GraphObj(InGraphObj)
{}

void FQuestTreeConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin,
	FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 1.5f;

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, /*inout*/ Params.WireThickness, /*inout*/ Params.WireColor);
	}
	
	// Params.AssociatedPin1 = OutputPin;
	// Params.AssociatedPin2 = InputPin;
	// Params.WireThickness = 1.5f;
	//
	// Params.WireColor = QuestTreeColors::Connection::Default;
	//
	// const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	// if (bDeemphasizeUnhoveredPins)
	// {
	// 	ApplyHoverDeemphasis(OutputPin, InputPin, /*inout*/ Params.WireThickness, /*inout*/ Params.WireColor);
	// }
	//
	// UQuestTreeGraphNode* FromNode = OutputPin ? Cast<UQuestTreeGraphNode>(OutputPin->GetOwningNode()) : NULL;
	// UQuestTreeGraphNode* ToNode = InputPin ? Cast<UQuestTreeGraphNode>(InputPin->GetOwningNode()) : NULL;
	// if (ToNode && FromNode)
	// {
	// 	// if ((ToNode->bDebuggerMarkCurrentlyActive && FromNode->bDebuggerMarkCurrentlyActive) ||
	// 	// 	(ToNode->bDebuggerMarkPreviouslyActive && FromNode->bDebuggerMarkPreviouslyActive))
	// 	// {
	// 	// 	Params.WireThickness = 10.0f;
	// 	// 	Params.bDrawBubbles = true;
	// 	// }
	// //	else
	// 	//if (FBehaviorTreeDebugger::IsPlaySessionPaused())
	// 	 {
	// 	 	UQuestTreeGraphNode* FirstToNode = ToNode;
	// 	 	int32 FirstPathIdx = ToNode->DebuggerSearchPathIndex;
	// 	 	for (int32 i = 0; i < ToNode->Decorators.Num(); i++)
	// 	 	{
	// 	 		UQuestTreeGraphNode* TestNode = ToNode->Decorators[i];
	// 	 		if (TestNode->DebuggerSearchPathIndex != INDEX_NONE &&
	// 	 			(TestNode->bDebuggerMarkSearchSucceeded || TestNode->bDebuggerMarkSearchFailed))
	// 	 		{
	// 	 			if (TestNode->DebuggerSearchPathIndex < FirstPathIdx || FirstPathIdx == INDEX_NONE)
	// 	 			{
	// 	 				FirstPathIdx = TestNode->DebuggerSearchPathIndex;
	// 	 				FirstToNode = TestNode;
	// 	 			}
	// 	 		}
	// 	 	}
	// 	
	// 	 	if (FirstToNode->bDebuggerMarkSearchSucceeded || FirstToNode->bDebuggerMarkSearchFailed)
	// 	 	{
	// 	 		Params.WireThickness = 5.0f;
	// 	 		Params.WireColor = FirstToNode->bDebuggerMarkSearchSucceeded ? QuestTreeColors::Debugger::SearchSucceeded :
	// 	 		QuestTreeColors::Debugger::SearchFailed;
	// 	
	// 	 		// Use the bUserFlag1 flag to indicate that we need to reverse the direction of connection (used by debugger)
	// 	 		Params.bUserFlag1 = true;
	// 	 	}
	// 	 }
	// }
}

void FQuestTreeConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries,
	FArrangedChildren& ArrangedNodes)
{
	// Build an acceleration structure to quickly find geometry for the nodes
	NodeWidgetMap.Empty();
	for (int32 NodeIndex = 0; NodeIndex < ArrangedNodes.Num(); ++NodeIndex)
	{
		FArrangedWidget& CurWidget = ArrangedNodes[NodeIndex];
		TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
		NodeWidgetMap.Add(ChildNode->GetNodeObj(), NodeIndex);
	}

	// Now draw
	FConnectionDrawingPolicy::Draw(InPinGeometries, ArrangedNodes);
}

void FQuestTreeConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom,
	const FConnectionParams& Params)
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	DrawSplineWithArrow(StartAnchorPoint, EndAnchorPoint, Params);
}

void FQuestTreeConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartPoint, const FVector2D& EndPoint,
	const FConnectionParams& Params)
{
	// bUserFlag1 indicates that we need to reverse the direction of connection (used by debugger)
	const FVector2D& P0 = Params.bUserFlag1 ? EndPoint : StartPoint;
	const FVector2D& P1 = Params.bUserFlag1 ? StartPoint : EndPoint;

	Internal_DrawLineWithArrow(P0, P1, Params);
}

void FQuestTreeConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint,
	const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionParams Params;
	DetermineWiringStyle(Pin, nullptr, /*inout*/ Params);

	if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
	{
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, EndPoint), EndPoint, Params);
	}
	else
	{
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, StartPoint), StartPoint, Params);
	}
}

FVector2D FQuestTreeConnectionDrawingPolicy::ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const
{
	const FVector2D Delta = End - Start;
	const FVector2D NormDelta = Delta.GetSafeNormal();

	return NormDelta;
}

void FQuestTreeConnectionDrawingPolicy::Internal_DrawLineWithArrow(const FVector2D& StartAnchorPoint,
	const FVector2D& EndAnchorPoint, const FConnectionParams& Params)
{
	//@TODO: Should this be scaled by zoom factor?
	const float LineSeparationAmount = 4.5f;

	const FVector2f StartAnchorPointTemp(UE::Slate::CastToVector2f(StartAnchorPoint));
	const FVector2f EndAnchorPointTemp(static_cast<float>(EndAnchorPoint.X), static_cast<float>(EndAnchorPoint.Y));
	const FVector2f DeltaPos = EndAnchorPointTemp - StartAnchorPointTemp;
	const FVector2f UnitDelta = DeltaPos.GetSafeNormal();
	const FVector2f Normal = FVector2f(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	// Come up with the final start/end points
	const FVector2f DirectionBias = Normal * LineSeparationAmount;
	const FVector2f LengthBias = ArrowRadius.X * UnitDelta;
	const FDeprecateSlateVector2D StartPoint = StartAnchorPointTemp + DirectionBias + LengthBias;
	const FDeprecateSlateVector2D EndPoint = EndAnchorPointTemp + DirectionBias - LengthBias;

	// Draw a line/spline
	DrawConnection(WireLayerID, FVector2D(StartPoint), FVector2D(EndPoint), Params);

	// Draw the arrow
	const FVector2f ArrowDrawPos = EndPoint - ArrowRadius;
	const float AngleInRadians = static_cast<float>(FMath::Atan2(DeltaPos.Y, DeltaPos.X));

	FSlateDrawElement::MakeRotatedBox(
		DrawElementsList,
		ArrowLayerID,
		FPaintGeometry(ArrowDrawPos, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
		ArrowImage,
		ESlateDrawEffect::None,
		AngleInRadians,
		TOptional<FVector2f>(),
		FSlateDrawElement::RelativeToElement,
		Params.WireColor
		);
}
