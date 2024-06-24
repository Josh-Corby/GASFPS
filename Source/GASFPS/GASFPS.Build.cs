// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GASFPS : ModuleRules
{
	public GASFPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "GameplayTags", "GameplayTasks", "GameplayAbilities", "AIModule", "ModularGameplay" ,"ModularGameplayActors" });

		PrivateDependencyModuleNames.AddRange(new string[] { "InputCore", "EnhancedInput", "NetCore"});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
