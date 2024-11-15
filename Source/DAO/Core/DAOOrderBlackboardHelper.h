// Craig Duthie 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DAOOrderBlackboardHelper.generated.h"

/**
 * 
 */
UCLASS()
class DAO_API UDAOOrderBlackboardHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    static const FName BLACKBOARD_KEY_ORDER_TYPE;
    static const FName BLACKBOARD_KEY_LOCATION;
    static const FName BLACKBOARD_KEY_TARGET;
    static const FName BLACKBOARD_KEY_INDEX;
    static const FName BLACKBOARD_KEY_RANGE;
    static const FName BLACKBOARD_KEY_HOME_LOCATION;
};
