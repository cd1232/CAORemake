// Craig Duthie 2024


#include "QuestTreeFactory.h"

#include "KatsuraQuestTree.h"

UQuestTreeFactory::UQuestTreeFactory(const FObjectInitializer& ObjectInitializer)
{
	SupportedClass = UKatsuraQuestTree::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UQuestTreeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(UKatsuraQuestTree::StaticClass()));
	return NewObject<UKatsuraQuestTree>(InParent, Class, Name, Flags);
}

bool UQuestTreeFactory::CanCreateNew() const
{
	return true;
}
