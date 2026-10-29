using UnrealBuildTool;
public class TerrainForgeToolset : ModuleRules
{
    public TerrainForgeToolset(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "TerrainForge", "ToolsetRegistry" });
        PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "Projects" });
    }
}
