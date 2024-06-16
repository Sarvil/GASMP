// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GASMP : ModuleRules
{
	public GASMP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
		PrivateDependencyModuleNames.AddRange(new string[] { "NetCore", "GameplayAbilities", "GameplayTags", "GameplayTasks" });
	}
}
