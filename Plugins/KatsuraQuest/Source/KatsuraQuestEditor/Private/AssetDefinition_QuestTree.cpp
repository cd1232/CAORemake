// Craig Duthie 2024


#include "AssetDefinition_QuestTree.h"

#include "KatsuraQuestEditorModule.h"
#include "KatsuraQuestTree.h"
#include "KatsuraQuestTreeEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"


FText UAssetDefinition_QuestTree::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_QuestTree", "Quest Tree");
}

FLinearColor UAssetDefinition_QuestTree::GetAssetColor() const
{
	return FColor(200,80,70);
}

TSoftClassPtr<UObject> UAssetDefinition_QuestTree::GetAssetClass() const
{
	return UKatsuraQuestTree::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_QuestTree::GetAssetCategories() const
{
	static const FAssetCategoryPath Categories[] = { EAssetCategoryPaths::Misc};
	return Categories;
}

EAssetCommandResult UAssetDefinition_QuestTree::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	const EToolkitMode::Type Mode = OpenArgs.GetToolkitMode();

	for (UKatsuraQuestTree* QuestTree : OpenArgs.LoadObjects<UKatsuraQuestTree>())
	{
		bool bFoundExisting = false;
		constexpr bool bFocusIfOpen = false;

		FKatsuraQuestTreeEditor* ExistingInstance = static_cast<FKatsuraQuestTreeEditor*>(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(QuestTree, bFocusIfOpen));
		if (ExistingInstance != nullptr && ExistingInstance->GetQuestTree() == nullptr)
		{
			ExistingInstance->InitQuestTreeEditor(Mode, OpenArgs.ToolkitHost, QuestTree);
			bFoundExisting = true;
		}

		if (!bFoundExisting)
		{
			FKatsuraQuestEditorModule& KatsuraQuestEditorModule = FModuleManager::GetModuleChecked<FKatsuraQuestEditorModule>("KatsuraQuestEditor");
			KatsuraQuestEditorModule.CreateQuestTreeEditor(Mode, OpenArgs.ToolkitHost, QuestTree);
		}
	}
	
	return EAssetCommandResult::Handled;
}

EAssetCommandResult UAssetDefinition_QuestTree::PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const
{
	return Super::PerformAssetDiff(DiffArgs);
}

void UAssetDefinition_QuestTree::OpenInDefaults(UKatsuraQuestTree* OldQuestTree,
	UKatsuraQuestTree* NewQuestTree) const
{
	FAssetDiffArgs DiffArgs;
	DiffArgs.OldAsset = OldQuestTree;
	DiffArgs.NewAsset = NewQuestTree;
	Super::PerformAssetDiff(DiffArgs);
}

#undef LOCTEXT_NAMESPACE
