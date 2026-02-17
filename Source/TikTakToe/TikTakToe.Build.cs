// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TikTakToe : ModuleRules
{
	public TikTakToe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemSteam"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
