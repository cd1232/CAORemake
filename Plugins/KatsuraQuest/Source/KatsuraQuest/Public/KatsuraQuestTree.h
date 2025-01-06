// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "QuestNode.h"
#include "UObject/Object.h"
#include "KatsuraQuestTree.generated.h"

UENUM(BlueprintType)
enum class EQuestState : uint8
{
	Unknown,
	Mentioned,
	Accepted,
	Achieved,
	Completed,
	Botched
};


#if WITH_EDITOR
class UKatsuraQuestTree;

/** Interface for quest tree graph interaction with the KatsuraQuestEditor module. */
class IQuestTreeGraphEditor
{
public:
	virtual ~IQuestTreeGraphEditor() {}

	/** Called when creating a new quest tree graph. */
	virtual UEdGraph* CreateNewQuestTreeGraph(UKatsuraQuestTree* InQuestTree) = 0;

	/** Sets up a quest node. */
	virtual void SetupQuestNode(UEdGraph* QuestTreeGraph, UQuestNode* QuestNode, bool bSelectNewNode) = 0;

	/** Compiles quest nodes from graph nodes. */
	virtual void CompileQuestNodesFromGraphNodes(UKatsuraQuestTree* QuestTree) = 0;
};
#endif

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStateUpdated, EQuestState, NewState);

/**
 * 
 */
UCLASS(BlueprintType)
class KATSURAQUEST_API UKatsuraQuestTree : public UObject
{
	GENERATED_BODY()

public:
	// TODO
	/** root node of loaded tree */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UQuestNode> RootNode;

	void InitializeInstance();

	UFUNCTION(BlueprintCallable)
	bool SetState(EQuestState InState);

	UFUNCTION(BlueprintPure)
	EQuestState GetState() const;

	UFUNCTION(BlueprintCallable)
	void BotchQuest();

	UFUNCTION(BlueprintCallable)
	void UnbotchQuest();

	TArray<UQuestNode*> GetCurrentActiveNodes() const;

#if WITH_EDITOR
	void SetupQuestNode(UQuestNode* InQuestNode, bool bSelectNewNode = true);

	static void SetQuestTreeGraphEditor(TSharedPtr<IQuestTreeGraphEditor> InQuestTreeGraphEditor);

	/** Use the EdGraph representation to compile the Quest Tree */
	void CompileQuestNodesFromGraphNodes();

	/** Gets the quest tree graph editor implementation. */
	static TSharedPtr<IQuestTreeGraphEditor> GetQuestTreeGraphEditor();

	UEdGraph* GetGraph() const;

	UFUNCTION(CallInEditor)
	void PrintQuestTree();
	void PrintChildren(UQuestNode* Node);

	/** Construct and initialize a node within this QuestTree */
	template<class T>
	T* ConstructQuestNode(TSubclassOf<UQuestNode> QuestNodeClass = T::StaticClass(), bool bSelectNewNode = true)
	{
		// Set flag to be transactional so it registers with undo system
		T* QuestNode = NewObject<T>(this, QuestNodeClass, NAME_None, RF_Transactional);
#if WITH_EDITOR
		AllNodes.Add(QuestNode);
		SetupQuestNode(QuestNode, bSelectNewNode);
#endif // WITH_EDITORONLY_DATA
		return QuestNode;
	}
	
#endif

protected:
	UFUNCTION()
	void OnNodeStateEntered(UQuestNode* QuestNode);
	
	UFUNCTION()
	void OnNodeStateExited(UQuestNode* QuestNode);

public:
	UPROPERTY(BlueprintAssignable)
	FOnQuestStateUpdated OnStateUpdated;

private:
#if WITH_EDITOR
	/** Ptr to interface to quest tree editor operations. */
	static TSharedPtr<IQuestTreeGraphEditor> QuestTreeGraphEditor;
#endif

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UQuestNode>> AllNodes;
	
	/** Graph for a Quest */
	UPROPERTY()
	TObjectPtr<class UEdGraph>	QuestGraph;

	/** Info about the graphs we last edited */
	UPROPERTY()
	TArray<FEditedDocumentInfo> LastEditedDocuments;
#endif

#if WITH_EDITOR
	friend class FKatsuraQuestTreeEditor;
	friend struct FQTGraphEditorSummoner;
#endif

	UPROPERTY(VisibleAnywhere)
	TArray<UQuestNode*> CurrentActiveNodes;
	
	UPROPERTY(VisibleAnywhere)
	EQuestState QuestState = EQuestState::Unknown;

	UPROPERTY(VisibleAnywhere)
	uint8 QuestStateBotched : 1;
};

