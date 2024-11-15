// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "RTSOrderTargetType.h"

#include "DAOBTDecorator_HasTargetType.generated.h"

class AAIController;

/**
 * 
 */
UCLASS(BlueprintType)
class DAO_API UDAOBTDecorator_HasTargetType : public UBTDecorator
{
	GENERATED_BODY()

public:
	UDAOBTDecorator_HasTargetType(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
	virtual void SetOwner(AActor* InActorOwner) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default")
	FBlackboardKeySelector TargetTypeKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default")
	ERTSOrderTargetType TypeToCheck;	

protected:
	/** Cached AIController owner of BehaviorTreeComponent. */
	UPROPERTY(Transient)
	TObjectPtr<AAIController> AIOwner;

	/** Cached AIController owner of BehaviorTreeComponent. */
	UPROPERTY(Transient)
	TObjectPtr<AActor> ActorOwner;


};
