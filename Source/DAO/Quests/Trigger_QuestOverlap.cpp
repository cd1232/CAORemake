// Craig Duthie 2024


#include "Trigger_QuestOverlap.h"

#include "Components/BrushComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/EmptyMessageStruct.h"


// Sets default values
ATrigger_QuestOverlap::ATrigger_QuestOverlap()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATrigger_QuestOverlap::BeginPlay()
{
	Super::BeginPlay();
	
	GetBrushComponent()->OnComponentBeginOverlap.AddDynamic(this, &ATrigger_QuestOverlap::OnOverlap);
}

void ATrigger_QuestOverlap::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDisableOverlapAfterTrigger)
	{
		GetBrushComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &ATrigger_QuestOverlap::OnOverlap);
	}

	if (TagToBroadcast.IsValid())
	{
		GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>()->BroadcastMessage(TagToBroadcast, FEmptyMessageStruct());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATrigger_QuestOverlap TagToBroadcast is null!"));
	}
}

