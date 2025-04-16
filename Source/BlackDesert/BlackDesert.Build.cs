// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BlackDesert : ModuleRules
{
	public BlackDesert(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "BlackDesert" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", 
			"GameplayAbilities", "GameplayTags", "GameplayTasks",
            "Niagara",
            "OnlineSubsystem",
            "OnlineSubsystemEOS",
            "OnlineSubsystemUtils",
            "Sockets",
            "Networking"
		});
	}
}
