// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Exo_CPlusPlus : ModuleRules
{
	public Exo_CPlusPlus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
