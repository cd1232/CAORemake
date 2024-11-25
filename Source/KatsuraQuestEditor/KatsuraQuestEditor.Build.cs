using UnrealBuildTool;

public class KatsuraQuestEditor : ModuleRules
{
    public KatsuraQuestEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "UnrealEd", "AIGraph",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "KatsuraQuest",
                "AssetDefinition",
                "GraphEditor",
                "AIGraph",
                "KismetWidgets",
                "ApplicationCore"
            }
        );
    }
}