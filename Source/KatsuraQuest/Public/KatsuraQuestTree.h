// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "QuestNode.h"
#include "UObject/Object.h"
#include "KatsuraQuestTree.generated.h"

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

	// /** Links graph nodes from sound nodes. */
	// virtual void LinkGraphNodesFromSoundNodes(USoundCue* SoundCue) = 0;
	//
	// /** Compiles sound nodes from graph nodes. */
	// virtual void CompileSoundNodesFromGraphNodes(USoundCue* SoundCue) = 0;
	//
	// /** Removes nodes which are null from the sound cue graph. */
	// virtual void RemoveNullNodes(USoundCue* SoundCue) = 0;
	//
	// /** Creates an input pin on the given sound cue graph node. */
	// virtual void CreateInputPin(UEdGraphNode* SoundCueNode) = 0;
	//
	// /** Renames all pins in a sound cue node */
	// virtual void RenameNodePins(USoundNode* SoundNode) = 0;
};
#endif


/**
 * 
 */
UCLASS()
class KATSURAQUEST_API UKatsuraQuestTree : public UObject
{
	GENERATED_BODY()

public:
	// TODO
	/** root node of loaded tree */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObject> RootNode;

#if WITH_EDITOR
	void SetupQuestNode(UQuestNode* InQuestNode, bool bSelectNewNode = true);

	static void SetQuestTreeGraphEditor(TSharedPtr<IQuestTreeGraphEditor> InQuestTreeGraphEditor);

	/** Gets the sound cue graph editor implementation. */
	static TSharedPtr<IQuestTreeGraphEditor> GetQuestTreeGraphEditor();

	UEdGraph* GetGraph() const;
	
#endif

	/** Construct and initialize a node within this Cue */
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
	
	
};

