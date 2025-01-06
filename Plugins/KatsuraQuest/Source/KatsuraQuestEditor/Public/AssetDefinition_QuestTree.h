// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "AssetDefinition_QuestTree.generated.h"

class UKatsuraQuestTree;

/**
 * 
 */
UCLASS()
class KATSURAQUESTEDITOR_API UAssetDefinition_QuestTree : public UAssetDefinitionDefault
{
	GENERATED_BODY()

protected:
	// UAssetDefinition Begin
	virtual FText GetAssetDisplayName() const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
	virtual EAssetCommandResult PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const override;
	// UAssetDefinition End

private:
	/* Called to open the Katsura Quest Tree defaults view, this opens whatever text diff tool the user has */
	void OpenInDefaults(class UKatsuraQuestTree* OldQuestTree, class UKatsuraQuestTree* NewQuestTree) const;
};
