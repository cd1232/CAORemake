// Craig Duthie 2023

#pragma once

#include "Containers/Ticker.h"

#include "GameUIManagerSubsystem.h"
#include "DAOUIManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API UDAOUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:
	UDAOUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();

	FTSTicker::FDelegateHandle TickHandle;	
};
