// Copyright Epic Games, Inc. All Rights Reserved.

#include "DAOGameplayAbilityTargetActor_Trace.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Core/DAOBaseCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOGameplayAbilityTargetActor_Trace)

// --------------------------------------------------------------------------------------------------------------------------------------------------------
//
//	AGameplayAbilityTargetActor_Trace
//
// --------------------------------------------------------------------------------------------------------------------------------------------------------

ADAOGameplayAbilityTargetActor_Trace::ADAOGameplayAbilityTargetActor_Trace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	MaxRange = 999999.0f;
}

void ADAOGameplayAbilityTargetActor_Trace::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ReticleActor.IsValid())
	{
		ReticleActor.Get()->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void ADAOGameplayAbilityTargetActor_Trace::LineTraceWithFilter(FHitResult& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
	check(World);

	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
	{
		const FHitResult& Hit = HitResults[HitIdx];

		if (!Hit.HitObjectHandle.IsValid() || FilterHandle.FilterPassesForActor(Hit.HitObjectHandle.FetchActor()))
		{
			OutHitResult = Hit;
			OutHitResult.bBlockingHit = true; // treat it as a blocking hit
			return;
		}
	}
}

void ADAOGameplayAbilityTargetActor_Trace::SweepWithFilter(FHitResult& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, const FQuat& Rotation, const FCollisionShape CollisionShape, FName ProfileName, const FCollisionQueryParams Params)
{
	check(World);

	TArray<FHitResult> HitResults;
	World->SweepMultiByProfile(HitResults, Start, End, Rotation, ProfileName, CollisionShape, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
	{
		const FHitResult& Hit = HitResults[HitIdx];

		if (!Hit.HitObjectHandle.IsValid() || FilterHandle.FilterPassesForActor(Hit.HitObjectHandle.FetchActor()))
		{
			OutHitResult = Hit;
			OutHitResult.bBlockingHit = true; // treat it as a blocking hit
			return;
		}
	}
}

void ADAOGameplayAbilityTargetActor_Trace::AimWithMouse(const AActor* InSourceActor, FCollisionQueryParams Params, FVector& OutTraceStart, FVector& OutTraceEnd) const
{
	if (!OwningAbility) // Server and launching client only
	{
		return;
	}

	ADAOBaseCharacter* OwningCharacter = Cast<ADAOBaseCharacter>(OwningAbility->GetCurrentActorInfo()->OwnerActor);
	APlayerController* PC = OwningCharacter->GetOwningPlayerController();
	check(PC);

	float mouseLocationX = 0.0f, mouseLocationY = 0.0f;
	PC->GetMousePosition(mouseLocationX, mouseLocationY);
	FVector2D mousePosition(mouseLocationX, mouseLocationY);

	FVector WorldOrigin;
	FVector WorldDirection;

	if (UGameplayStatics::DeprojectScreenToWorld(PC, mousePosition, WorldOrigin, WorldDirection) == true)
	{
		FHitResult HitResult;
		OutTraceStart = WorldOrigin;

		LineTraceWithFilter(HitResult, InSourceActor->GetWorld(), Filter, WorldOrigin, WorldOrigin + (WorldDirection * MaxRange), TraceProfile.Name, Params);

		const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(WorldOrigin, HitResult.Location) <= (MaxRange * MaxRange));
		
		OutTraceEnd = OutTraceStart + (WorldDirection * MaxRange);
	}
}

bool ADAOGameplayAbilityTargetActor_Trace::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition)
{
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	float DotToCenter = FVector::DotProduct(CameraToCenter, CameraDirection);
	if (DotToCenter >= 0)		//If this fails, we're pointed away from the center, but we might be inside the sphere and able to find a good exit point.
	{
		float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		float RadiusSquared = (AbilityRange * AbilityRange);
		if (DistanceSquared <= RadiusSquared)
		{
			float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			float DistanceAlongRay = DotToCenter + DistanceFromCamera;						//Subtracting instead of adding will get the other intersection point
			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);		//Cam aim point clipped to range sphere
			return true;
		}
	}
	return false;
}

void ADAOGameplayAbilityTargetActor_Trace::StartTargeting(UGameplayAbility* InAbility)
{
	Super::StartTargeting(InAbility);
	SourceActor = InAbility->GetCurrentActorInfo()->AvatarActor.Get();

	if (ReticleClass)
	{
		AGameplayAbilityWorldReticle* SpawnedReticleActor = GetWorld()->SpawnActor<AGameplayAbilityWorldReticle>(ReticleClass, GetActorLocation(), GetActorRotation());
		if (SpawnedReticleActor)
		{
			SpawnedReticleActor->InitializeReticle(this, PrimaryPC, ReticleParams);
			ReticleActor = SpawnedReticleActor;

			// This is to catch cases of playing on a listen server where we are using a replicated reticle actor.
			// (In a client controlled player, this would only run on the client and therefor never replicate. If it runs
			// on a listen server, the reticle actor may replicate. We want consistancy between client/listen server players.
			// Just saying 'make the reticle actor non replicated' isnt a good answer, since we want to mix and match reticle
			// actors and there may be other targeting types that want to replicate the same reticle actor class).
			if (!ShouldProduceTargetDataOnServer)
			{
				SpawnedReticleActor->SetReplicates(false);
			}
		}
	}
}

void ADAOGameplayAbilityTargetActor_Trace::Tick(float DeltaSeconds)
{
	// very temp - do a mostly hardcoded trace from the source actor
	if (SourceActor && SourceActor->GetLocalRole() != ENetRole::ROLE_SimulatedProxy)
	{
		FHitResult HitResult = PerformTrace(SourceActor);
		FVector EndPoint = HitResult.Component.IsValid() ? HitResult.ImpactPoint : HitResult.TraceEnd;

		//HitResult.

#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			DrawDebugLine(GetWorld(), SourceActor->GetActorLocation(), EndPoint, FColor::Green, false);
			DrawDebugSphere(GetWorld(), EndPoint, 16, 10, FColor::Green, false);
		}
#endif // ENABLE_DRAW_DEBUG

		SetActorLocationAndRotation(EndPoint, SourceActor->GetActorRotation());
	}
}

void ADAOGameplayAbilityTargetActor_Trace::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		bDebug = false;
		FGameplayAbilityTargetDataHandle Handle = MakeTargetData(PerformTrace(SourceActor));
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

FGameplayAbilityTargetDataHandle ADAOGameplayAbilityTargetActor_Trace::MakeTargetData(const FHitResult& HitResult) const
{
	/** Note: This will be cleaned up by the FGameplayAbilityTargetDataHandle (via an internal TSharedPtr) */
	return StartLocation.MakeTargetDataHandleFromHitResult(OwningAbility, HitResult);
}

