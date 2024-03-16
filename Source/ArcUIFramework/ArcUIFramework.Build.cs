// Copyright (C) Fabien Poupineau. All Rights Reserved.

using UnrealBuildTool;

// ReSharper disable once InconsistentNaming
public class ArcUIFramework : ModuleRules
{
	public ArcUIFramework(ReadOnlyTargetRules target) : base(target)
	{
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"DataRegistry",
				"Engine",
				"GameplayTags",
				"UMG",
				"CommonUI",
				"CommonInput",
				"DeveloperSettings",
				"StructUtils"
			}
		);
	}
}