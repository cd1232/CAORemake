// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "MinimapWidget.h"

#include "DAOMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API UDAOMinimapWidget : public UMinimapWidget
{
	GENERATED_BODY()

protected:
	virtual bool GetPlayerView(FVector& Location, FRotator& Rotation) const override;
};
