// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"

#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class FKatsuraQuestTreeEditor;

/**
 * 
 */
class FQuestTreeEditorApplicationMode : public FApplicationMode
{
public:
	FQuestTreeEditorApplicationMode(TSharedPtr<FKatsuraQuestTreeEditor> InQuestTreeEditor);

	virtual void RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

protected:
	TWeakPtr<FKatsuraQuestTreeEditor> QuestTreeEditor;

	// Set of spawnable tabs in quest tree editing mode
	FWorkflowAllowedTabSet QuestTreeEditorTabFactories;
};
