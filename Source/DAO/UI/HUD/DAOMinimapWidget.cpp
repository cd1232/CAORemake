// Craig Duthie 2024


#include "DAOMinimapWidget.h"

#include "Core/DAOBaseCharacter.h"
#include "Player/DAOPlayerController.h"

bool UDAOMinimapWidget::GetPlayerView(FVector& Location, FRotator& Rotation) const
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ADAOPlayerController* DAOPC = Cast<ADAOPlayerController>(PlayerController))
		{
			if (DAOPC->GetControlledCharacter())
			{
				FVector PlayerPosition = DAOPC->GetControlledCharacter()->GetActorLocation();
				//FRotator PlayerRotation = DAOPC->GetControlledCharacter()->GetActorRotation();
	
				DAOPC->GetPlayerViewPoint(Location, Rotation);
				Location = PlayerPosition;
			
				return true;
			}
		}
	}	

	return false;
}
