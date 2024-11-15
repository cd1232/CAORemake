// Craig Duthie 2024


#include "AI/DAOBTTask_UseAbility.h"

#include <GameFramework/Pawn.h>
#include <BehaviorTree/BlackboardComponent.h>

#include "RTSGameplayAbility.h"
#include "Abilities/DAOAbilitySystemComponent.h"
#include "RTSAbilitySystemHelper.h"
#include "RTSOrderBlackboardHelper.h"
#include "RTSOrderHelper.h"

UDAOBTTask_UseAbility::UDAOBTTask_UseAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Use Ability";
	// instantiating to be able to use Timers
	bCreateNodeInstance = true;
}


EBTNodeResult::Type UDAOBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MyOwnerComp = &OwnerComp;

	//FGameplayEventData EventData;
	//UDAOAbilitySystemHelper::CreateGameplayEventData(OrderedActor, TargetData, Ability->GetClass(), EventData);

	//int32 TriggeredAbilities = UDAOAbilitySystemHelper::SendGameplayEvent(OrderedActor, EventData);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!Pawn)
		return EBTNodeResult::Failed;

	AbilitySystem = Pawn->FindComponentByClass<UDAOAbilitySystemComponent>();
	if (!AbilitySystem)
		return EBTNodeResult::Failed;
	
	const TArray<TSubclassOf<UGameplayAbility>>& Abilities = AbilitySystem->GetInitialAndUnlockableAbilities();

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	
	const int AbilityIndex = BB->GetValueAsInt(URTSOrderBlackboardHelper::BLACKBOARD_KEY_INDEX);
	if (!Abilities.IsValidIndex(AbilityIndex))
		return EBTNodeResult::Failed;

	const TSubclassOf<UGameplayAbility> AbilityClass = Abilities[AbilityIndex];
	
	MyAbility = AbilityClass->GetDefaultObject<UGameplayAbility>();

	const FVector Location = FVector(BB->GetValueAsVector(URTSOrderBlackboardHelper::BLACKBOARD_KEY_LOCATION));
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(URTSOrderBlackboardHelper::BLACKBOARD_KEY_TARGET));
	
	const FRTSOrderTargetData TargetData = URTSOrderHelper::CreateOrderTargetData(Pawn, Target, Location);

	FGameplayEventData EventData;
	URTSAbilitySystemHelper::CreateGameplayEventData(Pawn, TargetData, MyAbility->GetClass(), EventData);

	AbilitySystem->OnGameplayAbilityEnded.AddDynamic(this, &UDAOBTTask_UseAbility::OnAbilityEnded);
		
	if (AbilitySystem->TryActivateAbilityByClassWithEventData(AbilityClass, EventData))
	{
		return EBTNodeResult::InProgress;
	}
	
	AbilitySystem->OnGameplayAbilityEnded.RemoveDynamic(this, &UDAOBTTask_UseAbility::OnAbilityEnded);
	
	// int32 TriggeredAbilities = URTSAbilitySystemHelper::SendGameplayEvent(Pawn, EventData);
	// if (TriggeredAbilities > 0)
	// {
	// 	MyAbilitySystem->OnGameplayAbilityEnded.RemoveDynamic(this, &URTSBTTask_UseAbility::OnAbilityEnded);
	// 	MyAbilitySystem->OnGameplayAbilityEnded.AddDynamic(this, &URTSBTTask_UseAbility::OnAbilityEnded);
	// 	return EBTNodeResult::InProgress;
	// }
	
	return EBTNodeResult::Failed;

}


EBTNodeResult::Type UDAOBTTask_UseAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AbilitySystem != nullptr && MyAbility != nullptr)
	{
		AbilitySystem->CancelAbility(MyAbility);
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}


void UDAOBTTask_UseAbility::OnAbilityEnded(TSubclassOf<UGameplayAbility> Ability, FGameplayAbilitySpecHandle AbilitySpecHandle, bool bWasCancelled)
{
	if (AbilitySystem)
	{
		AbilitySystem->OnGameplayAbilityEnded.RemoveDynamic(this, &UDAOBTTask_UseAbility::OnAbilityEnded);
	}

	if (MyOwnerComp)
	{
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
	}
}