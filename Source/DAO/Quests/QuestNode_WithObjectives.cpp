// Craig Duthie 2024

#include "QuestNode_WithObjectives.h"

#include "GameFramework/GameplayMessageSubsystem.h"

void UQuestNode_WithObjectives::EnterState()
{
	Super::EnterState();

	OnNewObjectives.Broadcast(QuestObjectives);

	GetWorld()->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>()->BroadcastMessage(FGameplayTag::RequestGameplayTag(TEXT("Quest.Objective.Enter")), FQuestObjectiveMessage(QuestObjectives));

}
