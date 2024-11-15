
#include "DAOAbilityTask_WaitTargetData.h"

#include "AbilitySystemComponent.h"
#include "Core/DAOBaseCharacter.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOAbilityTask_WaitTargetData)


UDAOAbilityTask_WaitTargetData::UDAOAbilityTask_WaitTargetData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}


UDAOAbilityTask_WaitTargetData* UDAOAbilityTask_WaitTargetData::WaitTargetData_DAO(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
TSubclassOf<AGameplayAbilityTargetActor> InTargetClass)
{
	UDAOAbilityTask_WaitTargetData* MyObj = NewAbilityTask<UDAOAbilityTask_WaitTargetData>(OwningAbility, TaskInstanceName);		//Register for task list here, providing a given FName as a key
	MyObj->TargetClass = InTargetClass;
	MyObj->TargetActor = nullptr;
	MyObj->ConfirmationType = ConfirmationType;
	return MyObj;
}

UDAOAbilityTask_WaitTargetData* UDAOAbilityTask_WaitTargetData::WaitTargetDataUsingActor_DAO(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, 
AGameplayAbilityTargetActor* InTargetActor)
{
	UDAOAbilityTask_WaitTargetData* MyObj = NewAbilityTask<UDAOAbilityTask_WaitTargetData>(OwningAbility, TaskInstanceName);		//Register for task list here, providing a given FName as a key
	MyObj->TargetClass = nullptr;
	MyObj->TargetActor = InTargetActor;
	MyObj->ConfirmationType = ConfirmationType;
	return MyObj;
}

void UDAOAbilityTask_WaitTargetData::InitializeTargetActor(AGameplayAbilityTargetActor* SpawnedActor) const
{
	check(SpawnedActor);
	check(Ability);

	ADAOBaseCharacter* OwningCharacter = Cast<ADAOBaseCharacter>(Ability->GetCurrentActorInfo()->OwnerActor);
	APlayerController* PC = OwningCharacter->GetOwningPlayerController();
	check(PC);

	SpawnedActor->PrimaryPC = PC;

	// If we spawned the target actor, always register the callbacks for when the data is ready.
	SpawnedActor->TargetDataReadyDelegate.AddUObject(const_cast<UDAOAbilityTask_WaitTargetData*>(this), &UAbilityTask_WaitTargetData::OnTargetDataReadyCallback);
	SpawnedActor->CanceledDelegate.AddUObject(const_cast<UDAOAbilityTask_WaitTargetData*>(this), &UAbilityTask_WaitTargetData::OnTargetDataCancelledCallback);
}
