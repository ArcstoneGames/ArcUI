// Copyright (C) Fabien Poupineau. All Rights Reserved.

using UnrealBuildTool;

// ReSharper disable once InconsistentNaming
public class ArcUICommon : ModuleRules
{
    public ArcUICommon(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "UMG",
                "CommonUI",
                "CommonInput",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}