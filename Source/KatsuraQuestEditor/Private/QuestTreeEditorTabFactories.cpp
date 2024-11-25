// Craig Duthie 2024


#include "QuestTreeEditorTabFactories.h"

#include "QuestTreeEditorTabs.h"

#include "KatsuraQuestTreeEditor.h"
#include "Containers/Array.h"
#include "Engine/Blueprint.h"
#include "GraphEditor.h"
#include "KatsuraQuestTree.h"
#include "HAL/PlatformCrt.h"
#include "Internationalization/Internationalization.h"
#include "Math/Vector2D.h"
#include "Misc/AssertionMacros.h"
#include "Styling/AppStyle.h"
#include "Styling/ISlateStyle.h"
#include "Textures/SlateIcon.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "QuestTreeEditorFactories"

FQuestTreeDetailsSummoner::FQuestTreeDetailsSummoner(TSharedPtr<FKatsuraQuestTreeEditor> InQuestTreeEditorPtr)
: FWorkflowTabFactory(FQuestTreeEditorTabs::GraphDetailsID, InQuestTreeEditorPtr)
, QuestTreeEditorPtr(InQuestTreeEditorPtr)
{
	TabLabel = LOCTEXT("QuestTreeDetailsLabel", "Details");
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("QuestTreeDetailsView", "Details");
	ViewMenuTooltip = LOCTEXT("QuestTreeDetailsView_ToolTip", "Show the details view");
}

TSharedRef<SWidget> FQuestTreeDetailsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(QuestTreeEditorPtr.IsValid());
	return QuestTreeEditorPtr.Pin()->SpawnProperties();
}

FText FQuestTreeDetailsSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("QuestTreeDetailsTabTooltip", "The Quest tree details tab allows editing of the properties of quest tree nodes");
}

FQTGraphEditorSummoner::FQTGraphEditorSummoner(TSharedPtr<FKatsuraQuestTreeEditor> InQuestTreeEditorPtr,
                                               FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
	: FDocumentTabFactoryForObjects<UEdGraph>(FQuestTreeEditorTabs::GraphEditorID, InQuestTreeEditorPtr)
	, QuestTreeEditorPtr(InQuestTreeEditorPtr)
	, OnCreateGraphEditorWidget(CreateGraphEditorWidgetCallback)
{}

void FQTGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	check(QuestTreeEditorPtr.IsValid());
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	QuestTreeEditorPtr.Pin()->OnGraphEditorFocused(GraphEditor);
}

void FQTGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->NotifyGraphChanged();
}

TAttribute<FText> FQTGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute<FText>( FText::FromString( DocumentID->GetName() ) );
}

TSharedRef<SWidget> FQTGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info,
	UEdGraph* DocumentID) const
{
	return OnCreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush* FQTGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info,
	UEdGraph* DocumentID) const
{
	return FAppStyle::Get().GetBrush("BTEditor.BehaviorTree");
}

void FQTGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	check(QuestTreeEditorPtr.IsValid());
	check(QuestTreeEditorPtr.Pin()->GetQuestTree());

	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());

	FVector2D ViewLocation;
	float ZoomAmount;
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);

	UEdGraph* Graph = FTabPayload_UObject::CastChecked<UEdGraph>(Payload);
	QuestTreeEditorPtr.Pin()->GetQuestTree()->LastEditedDocuments.Add(FEditedDocumentInfo(Graph, ViewLocation, ZoomAmount));
}

#undef LOCTEXT_NAMESPACE
