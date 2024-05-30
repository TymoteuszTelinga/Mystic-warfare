// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MysticWarfare : ModuleRules
{
	public MysticWarfare(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "AnimGraphRuntime" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "OnlineSubsystem",
            "OnlineSubsystemUtils" });
	}
}
