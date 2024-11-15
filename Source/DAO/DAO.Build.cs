// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DAO : ModuleRules
{
	public DAO(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
		new string[] {
			"DAO",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "LyraInventory" });

		PublicDependencyModuleNames.AddRange(
			new string[] { 
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
				"NavigationSystem", 
				"AIModule", 
				"Niagara", 
				"EnhancedInput",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"CommonGame",
				"ModularGameplayActors",
				"UMG",
				"SlateCore",
				"GameplayMessageRuntime",
				"ModularGameplay",
				"NetCore",
				"DeveloperSettings",
				"TargetingSystem",
				"RTSPlugin",
				"PocketWorlds",
				"Minimap"
			});

		SetupIrisSupport(Target);
	}
}
