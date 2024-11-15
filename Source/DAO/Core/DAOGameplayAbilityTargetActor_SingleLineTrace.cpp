// Copyright Epic Games, Inc. All Rights Reserved.

#include "DAOGameplayAbilityTargetActor_SingleLineTrace.h"
#include "DrawDebugHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOGameplayAbilityTargetActor_SingleLineTrace)

// --------------------------------------------------------------------------------------------------------------------------------------------------------
//
//	AGameplayAbilityTargetActor_SingleLineTrace
//
// --------------------------------------------------------------------------------------------------------------------------------------------------------

ADAOGameplayAbilityTargetActor_SingleLineTrace::ADAOGameplayAbilityTargetActor_SingleLineTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool ADAOGameplayAbilityTargetActor_SingleLineTrace::IsConfirmTargetingAllowed()
{
	// TODO Craig
	return ValidActor.IsValid();
}

FHitResult ADAOGameplayAbilityTargetActor_SingleLineTrace::PerformTrace(AActor* InSourceActor)
{
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(InSourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ADAOGameplayAbilityTargetActor_SingleLineTrace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = FVector::ZeroVector;// InSourceActor->GetActorLocation();
	FVector TraceEnd = FVector::ZeroVector;
	//AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd);		//Effective on server and launching client only
	
	AimWithMouse(InSourceActor, Params, TraceStart, TraceEnd);

	// ------------------------------------------------------

	FHitResult ReturnHitResult;
	LineTraceWithFilter(ReturnHitResult, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);
	//Default to end of trace line if we don't hit anything.
	if (!ReturnHitResult.bBlockingHit)
	{
		ReturnHitResult.Location = TraceEnd;
	}

	const bool bHitActor = (ReturnHitResult.bBlockingHit && (ReturnHitResult.HitObjectHandle.IsValid()));
	if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
	{
		const FVector ReticleLocation = (bHitActor && LocalReticleActor->bSnapToTargetedActor)
			? ReturnHitResult.HitObjectHandle.GetLocation()
			: ReturnHitResult.Location;

		LocalReticleActor->SetActorLocation(ReticleLocation);
		LocalReticleActor->SetIsTargetAnActor(bHitActor);
	}

	if (bHitActor)
	{
		ValidActor = ReturnHitResult.HitObjectHandle.GetCachedActor();
		//GetWorld()->GetFirstPlayerController<APlayerController>()->CurrentMouseCursor = EMouseCursor::CardinalCross;
	}
	else
	{
		//GetWorld()->GetFirstPlayerController<APlayerController>()->CurrentMouseCursor = EMouseCursor::Default;
		ValidActor.Reset();
	}

#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green);
		DrawDebugSphere(GetWorld(), TraceEnd, 100.0f, 16, FColor::Green);
	}
#endif // ENABLE_DRAW_DEBUG
	return ReturnHitResult;
}

