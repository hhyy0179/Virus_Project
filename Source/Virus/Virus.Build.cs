// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Virus : ModuleRules
{
	public Virus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Sockets", "UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] { "SlateCore", "Slate" });
    }
}
