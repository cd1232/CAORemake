// Craig Duthie 2024


#include "QuestTreeEditorApplicationMode.h"

#include "QuestTreeEditorTabs.h"
#include "KatsuraQuestTreeEditor.h"
#include "QuestTreeEditorTabFactories.h"


FQuestTreeEditorApplicationMode::FQuestTreeEditorApplicationMode(TSharedPtr<FKatsuraQuestTreeEditor> InQuestTreeEditor)
: FApplicationMode(FKatsuraQuestTreeEditor::QuestTreeMode, FKatsuraQuestTreeEditor::GetLocalizedMode)
{
	QuestTreeEditor = InQuestTreeEditor;

	QuestTreeEditorTabFactories.RegisterFactory(MakeShareable(new FQuestTreeDetailsSummoner(InQuestTreeEditor)));
	
	TabLayout = FTabManager::NewLayout( "Standalone_QuestTree_Layout_v2" )
	->AddArea
	(
		FTabManager::NewPrimaryArea() ->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewSplitter() ->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
				->AddTab(FQuestTreeEditorTabs::GraphEditorID, ETabState::ClosedTab)
			)
			->Split
			(
				FTabManager::NewSplitter() ->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.3f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->AddTab(FQuestTreeEditorTabs::GraphDetailsID, ETabState::OpenedTab)
				)
			)
		)
	);

	// TODO
	//InBehaviorTreeEditor->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
	//InBehaviorTreeEditor->GetToolbarBuilder()->AddDebuggerToolbar(ToolbarExtender);
	//InBehaviorTreeEditor->GetToolbarBuilder()->AddBehaviorTreeToolbar(ToolbarExtender);
}

void FQuestTreeEditorApplicationMode::RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager)
{
	check(QuestTreeEditor.IsValid());
	TSharedPtr<FKatsuraQuestTreeEditor> QuestTreeEditorPtr = QuestTreeEditor.Pin();

	// TODO
	//QuestTreeEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());

	// Mode-specific setup
	QuestTreeEditorPtr->PushTabFactories(QuestTreeEditorTabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FQuestTreeEditorApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
	
	check(QuestTreeEditor.IsValid());
	TSharedPtr<FKatsuraQuestTreeEditor> QuestTreeEditorPtr = QuestTreeEditor.Pin();

	// TODO
	//QuestTreeEditorPtr->SaveEditedObjectState();
}

void FQuestTreeEditorApplicationMode::PostActivateMode()
{
	// Reopen any documents that were open when the blueprint was last saved
	check(QuestTreeEditor.IsValid());
	TSharedPtr<FKatsuraQuestTreeEditor> QuestTreeEditorPtr = QuestTreeEditor.Pin();
	QuestTreeEditorPtr->RestoreQuestTree();

	FApplicationMode::PostActivateMode();
}
