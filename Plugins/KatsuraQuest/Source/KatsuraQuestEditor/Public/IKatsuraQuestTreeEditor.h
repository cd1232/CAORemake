// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UEdGraphNode;

/**
 * 
 */
class KATSURAQUESTEDITOR_API IKatsuraQuestTreeEditor : public FWorkflowCentricApplication
{
public:
	// TODO
	//virtual void InitializeDebuggerState(class FBehaviorTreeDebugger* ParentDebugger) const = 0;
	virtual UEdGraphNode* FindInjectedNode(int32 Index) const = 0;
	virtual void DoubleClickNode(UEdGraphNode* Node) = 0;
	virtual void FocusAttentionOnNode(UEdGraphNode* Node) = 0;
};
