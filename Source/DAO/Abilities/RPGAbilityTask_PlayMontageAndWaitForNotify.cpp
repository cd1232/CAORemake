// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAbilityTask_PlayMontageAndWaitForNotify.h"

#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"

URPGAbilityTask_PlayMontageAndWaitForNotify::URPGAbilityTask_PlayMontageAndWaitForNotify(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

UAbilitySystemComponent* URPGAbilityTask_PlayMontageAndWaitForNotify::GetTargetASC()
{
	return AbilitySystemComponent.Get();
}

void URPGAbilityTask_PlayMontageAndWaitForNotify::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			// Reset AnimRootMotionTranslationScale
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority ||
							  (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}

		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FName());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FName());
		}
	}
}

void URPGAbilityTask_PlayMontageAndWaitForNotify::OnAbilityCancelled()
{
	if (StopPlayingMontage())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FName());
		}
	}
}

void URPGAbilityTask_PlayMontageAndWaitForNotify::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FName());
		}
	}

	EndTask();
}

URPGAbilityTask_PlayMontageAndWaitForNotify* URPGAbilityTask_PlayMontageAndWaitForNotify::PlayMontageAndWaitForNotify(UGameplayAbility* OwningAbility,
	FName TaskInstanceName, UAnimMontage* MontageToPlay, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	URPGAbilityTask_PlayMontageAndWaitForNotify* MyObj = NewAbilityTask<URPGAbilityTask_PlayMontageAndWaitForNotify>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return MyObj;
}

void URPGAbilityTask_PlayMontageAndWaitForNotify::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	OnNotify.Broadcast(NotifyName);
}

void URPGAbilityTask_PlayMontageAndWaitForNotify::OnNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	// TODO
}

void URPGAbilityTask_PlayMontageAndWaitForNotify::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;
	UAbilitySystemComponent* RPGAbilitySystemComponent = GetTargetASC();

	if (RPGAbilitySystemComponent)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{			
			if (RPGAbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &URPGAbilityTask_PlayMontageAndWaitForNotify::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &URPGAbilityTask_PlayMontageAndWaitForNotify::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &URPGAbilityTask_PlayMontageAndWaitForNotify::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &URPGAbilityTask_PlayMontageAndWaitForNotify::OnNotifyBegin);
				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &URPGAbilityTask_PlayMontageAndWaitForNotify::OnNotifyEnd);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
								  (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			//ABILITY_LOG(Warning, TEXT("URPGAbilityTask_PlayMontageAndWaitForNotify call to PlayMontage failed!"));
		}
	}
	else
	{
		//ABILITY_LOG(Warning, TEXT("URPGAbilityTask_PlayMontageAndWaitForNotify called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		//ABILITY_LOG(Warning, TEXT("URPGAbilityTask_PlayMontageAndWaitForNotify called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),*InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FName());
		}
	}

	SetWaitingOnAvatar();
}

void URPGAbilityTask_PlayMontageAndWaitForNotify::ExternalCancel()
{
	check(AbilitySystemComponent.IsValid());

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void URPGAbilityTask_PlayMontageAndWaitForNotify::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	UAbilitySystemComponent* RPGAbilitySystemComponent = GetTargetASC();

	if (RPGAbilitySystemComponent)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->OnPlayMontageNotifyBegin.RemoveAll(this);
			AnimInstance->OnPlayMontageNotifyEnd.RemoveAll(this);
		}
	}

	Super::OnDestroy(AbilityEnded);

}

bool URPGAbilityTask_PlayMontageAndWaitForNotify::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilitySystemComponent.IsValid() && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

FString URPGAbilityTask_PlayMontageAndWaitForNotify::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForNotify. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}
