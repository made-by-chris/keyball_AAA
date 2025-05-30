// Source\keyball_AAA\keyball_AAA.Build.cs
using UnrealBuildTool;

public class keyball_AAA : ModuleRules
{
    public keyball_AAA(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
    
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Add platform-specific dependencies for KeyboardLayoutDetector
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // PublicSystemLibraries.Add("user32.lib");
            // Explicitly add Windows SDK library path
            // PublicSystemLibraryPaths.Add(@"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicFrameworks.Add("Carbon");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicSystemLibraries.AddRange(new string[] { "X11", "XKB" });
        }

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        
        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}