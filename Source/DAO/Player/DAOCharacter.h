// Craig Duthie 2023

#pragma once

// Engine Includes

// Local Includes
#include "Core/DAOBaseCharacter.h"

// Generated Include
#include "DAOCharacter.generated.h"

// Forward Declares
class UCameraComponent;
class USpringArmComponent;
class UCharacterInfoDataAsset;

UCLASS(Blueprintable)
class DAO_API ADAOCharacter : public ADAOBaseCharacter
{
	GENERATED_BODY()

public:
	ADAOCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
};

