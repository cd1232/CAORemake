// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "QuestTreeFactory.generated.h"

/**
 * 
 */
UCLASS()
class KATSURAQUESTEDITOR_API UQuestTreeFactory : public UFactory
{
	GENERATED_BODY()

public:
	UQuestTreeFactory(const FObjectInitializer& ObjectInitializer);
	
	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class,UObject* InParent,FName Name,EObjectFlags Flags,UObject* Context,FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override;
	// End of UFactory interface
};
