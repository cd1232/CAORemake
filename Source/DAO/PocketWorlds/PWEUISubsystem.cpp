// MIT License
//
// Copyright (c) 2022 ISMA FILO
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include "PWEUISubsystem.h"
#include "PocketLevelSystem.h"
#include "PWEPocketStage.h"
#include "PocketLevel.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"


void UPWEUISubsystem::SetPocketCapture(UPocketCapture* NewPocketCapture)
{
	PocketCapture = NewPocketCapture;
}

UPocketCapture* UPWEUISubsystem::GetPocketCapture()
{
	return PocketCapture;
}

void UPWEUISubsystem::SetPocketStage(APWEPocketStage* NewPocketStage)
{
	PocketStage = NewPocketStage;
}

APWEPocketStage* UPWEUISubsystem::GetPocketStage()
{
	return PocketStage;
}

void UPWEUISubsystem::SpawnPocketLevel(FSoftObjectPath PocketLevelAssetPath)
{
	UPocketLevelSubsystem* PocketLevelSubsystem = GetWorld()->GetSubsystem<UPocketLevelSubsystem>();

	UPocketLevel* PocketLevel = Cast<UPocketLevel>(PocketLevelAssetPath.TryLoad());
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstPlayerController()->GetLocalPlayer();

	FVector PocketLevelLocation = FVector(0, 0, -10000); //Default location in case the level has no Postpo
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);
	if (!FoundActors.IsEmpty())
	{ 
		const APostProcessVolume* PPVolume = Cast<APostProcessVolume>(FoundActors[0]);
		const FBoxSphereBounds Bounds = PPVolume->GetBounds();

		PocketLevelLocation = PPVolume->GetActorLocation() + FVector(0, 0, -Bounds.BoxExtent.Z) + PocketLevelLocation;
	}

	PocketLevelSubsystem->GetOrCreatePocketLevelFor(LocalPlayer, PocketLevel, PocketLevelLocation);
}