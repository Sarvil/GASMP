// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GASMP : ModuleRules
{
	public GASMP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineSubsystemSteam", "OnlineSubsystem", "NetCore", "MotionWarping", "Niagara" });
		PublicIncludePaths.Add("GASMP/");
		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });
	}
}
