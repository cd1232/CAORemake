// Craig Duthie 2023


// This Include
#include "DAOSaveGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOSaveGame)

void UDAOSaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsLoading() && SavedDataVersion != EDAOSaveGameVersion::LatestVersion)
	{
		// TODO
		// Convert stuff if needed
	}
}
