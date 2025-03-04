// Copyright U.N.Owen, All Rights Reserved

using UnrealBuildTool;

public class GameplayFramework : ModuleRules
{
	public GameplayFramework(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EnhancedInput",
				"InputCore",
				"UMG",
				"Slate",
				"SlateCore",
				"GameplayTags",
				"DeveloperSettings"
				// ... add other public dependencies that you statically link with here ...
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
				// ... add private dependencies that you statically link with here ...
			}
			);
	}
}
