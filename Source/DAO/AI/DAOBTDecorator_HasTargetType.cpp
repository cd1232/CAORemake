// Craig Duthie 2024


#include "AI/DAOBTDecorator_HasTargetType.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "RTSOrder.h"


UDAOBTDecorator_HasTargetType::UDAOBTDecorator_HasTargetType(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
}

bool UDAOBTDecorator_HasTargetType::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	int32 AbilityIndex = BlackboardComp->GetValueAsInt(FName(TEXT("Order_Index")));
	
	UClass* Class = BlackboardComp->GetValueAsClass(TargetTypeKey.SelectedKeyName);
	if (Class->IsChildOf(URTSOrder::StaticClass()))
	{
		UObject* CDO = Class->ClassDefaultObject;
		if (CDO)
		{
			if (URTSOrder* Order = Cast<URTSOrder>(CDO))
			{
				return TypeToCheck == Order->GetTargetType(AIOwner->GetPawn(), AbilityIndex);
			}
		}
	}

	return false;
}

void UDAOBTDecorator_HasTargetType::SetOwner(AActor* InActorOwner)
{
	ActorOwner = InActorOwner;
	AIOwner = Cast<AAIController>(InActorOwner);
}


