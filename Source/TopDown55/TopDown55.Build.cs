// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TopDown55 : ModuleRules
{
	public TopDown55(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		if (Target.bBuildEditor)
			OptimizeCode = CodeOptimization.Never;

        PublicDependencyModuleNames.AddRange(new string[] { 
	        "Core",
	        "CoreUObject",
	        "Engine",
	        "InputCore",
	        "NavigationSystem",
	        "AIModule",
	        "Niagara",
	        "EnhancedInput",
	        "TheDarkWestTest",
	        "GameplayAbilities"
        });
    }
}
