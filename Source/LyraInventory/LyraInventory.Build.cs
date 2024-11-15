using UnrealBuildTool;

public class LyraInventory : ModuleRules
{
    public LyraInventory(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "GameplayMessageRuntime", "ModularGameplay", "GameplayAbilities"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "ModularGameplay",
                "NetCore",
                "DeveloperSettings",
                "AIModule",
                "GameplayTags"
            }
        );
        
        SetupIrisSupport(Target);
    }
}