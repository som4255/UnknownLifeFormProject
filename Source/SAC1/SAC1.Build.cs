// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SAC1 : ModuleRules
{
	public SAC1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
            "AIModule",
            "GameplayTasks",
            "NavigationSystem",
            "UMG",
            "CinematicCamera",
            "HeadMountedDisplay", 
			"EnhancedInput",
            "Niagara"
        });
	}
}
