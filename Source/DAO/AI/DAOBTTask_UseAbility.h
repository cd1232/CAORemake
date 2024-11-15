// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "DAOBTTask_UseAbility.generated.h"

class UDAOAbilitySystemComponent;
class UGameplayAbility;
class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class DAO_API UDAOBTTask_UseAbility : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UDAOBTTask_UseAbility(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UFUNCTION()
	void OnAbilityEnded(TSubclassOf<UGameplayAbility> Ability, FGameplayAbilitySpecHandle AbilitySpecHandle, bool bWasCancelled);

protected:
	UPROPERTY()
	TObjectPtr<UDAOAbilitySystemComponent> AbilitySystem;

	UPROPERTY()
	TObjectPtr<UGameplayAbility> MyAbility;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> MyOwnerComp;

};
