// Craig Duthie 2024


#include "QuestManagerSubsystem.h"

#include "KatsuraQuestTree.h"
#include "QuestNode.h"

bool UQuestManagerSubsystem::StartQuest(UKatsuraQuestTree* QuestTree)
{
	UQuestNode* CDORootNode = QuestTree->RootNode;
	if (CDORootNode)
	{
		UKatsuraQuestTree* QuestTreeInstance = NewObject<UKatsuraQuestTree>(this, UKatsuraQuestTree::StaticClass(), NAME_None, RF_NoFlags, QuestTree);
		QuestTreeInstance->InitializeInstance();

		UQuestNode* RootNodeInstance = QuestTreeInstance->RootNode;
		StartedQuests.Add(QuestTreeInstance);
		RootNodeInstance->EnterState();
		QuestTree->SetState(EQuestState::Accepted);

		OnQuestUpdated.Broadcast(QuestTreeInstance, EQuestState::Accepted);
		
		return true;
	}
	
	return false;
}

void UQuestManagerSubsystem::SaveQuests()
{
}

void UQuestManagerSubsystem::LoadQuests()
{
}
