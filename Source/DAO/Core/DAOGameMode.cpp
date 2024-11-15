// Craig Duthie 2023

// This Include
#include "DAOGameMode.h"

// Engine Includes
#include <UObject/ConstructorHelpers.h>

// Local Includes
#include "Player/DAOPlayerController.h"
#include "Player/DAOCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DAOGameMode)

ADAOGameMode::ADAOGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ADAOPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Base/Blueprints/BP_PlayerCharacter"));
	//if (PlayerPawnBPClass.Class != nullptr)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}

	//// set default controller to our Blueprinted controller
	//static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Base/Blueprints/BP_PlayerController"));
	//if(PlayerControllerBPClass.Class != NULL)
	//{
	//	PlayerControllerClass = PlayerControllerBPClass.Class;
	//}
}