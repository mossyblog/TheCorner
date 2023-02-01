// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheCorner : ModuleRules
{
	public TheCorner(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "ApparatusRuntime" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "MaulProtoPrecompiled.h";
		MinFilesUsingPrecompiledHeaderOverride = 1;
		bUseUnity = false;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
    }
}
