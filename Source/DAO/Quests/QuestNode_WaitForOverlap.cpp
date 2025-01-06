// Craig Duthie 2024


#include "QuestNode_WaitForOverlap.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/EmptyMessageStruct.h"

void UQuestNode_WaitForOverlap::EnterState()
{
	Super::EnterState();

	GetWorld()->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>()->RegisterListener<FEmptyMessageStruct>(TagToListenFor, this, &UQuestNode_WaitForOverlap::OnOverlap);
}

void UQuestNode_WaitForOverlap::ExitState()
{
	Super::ExitState();
}

void UQuestNode_WaitForOverlap::OnOverlap(FGameplayTag BroadcastedTag, const FEmptyMessageStruct& Payload)
{
	UE_LOG(LogTemp, Log, TEXT("Overlap happened!"));
	if (BroadcastedTag.MatchesTagExact(TagToListenFor))
	{
		ExitState();
	}
}
