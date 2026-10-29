#pragma once
#include "CoreMinimal.h"
#include "TerrainForgeTypes.h"
#include "TerrainForgeGraphTypes.h"
#include "TerrainForgePresets.h"
#include "ToolsetRegistry/ToolsetDefinition.h"
#include "ToolsetRegistry/ToolCallAsyncResult.h"
#include "TerrainForgeToolset.generated.h"

UCLASS(BlueprintType)
class TERRAINFORGETOOLSET_API UToolCallAsyncResultTerrain : public UToolCallAsyncResult
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category="TerrainForge") FTerrainStatus Value;
    bool SetValue(const FTerrainStatus& In){return MaybeBroadcastSuccessfulCompletion(FTerrainStatus(In),Value);}
};
UCLASS(BlueprintType)
class TERRAINFORGETOOLSET_API UToolCallAsyncResultTerrainImport : public UToolCallAsyncResult
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category="TerrainForge") FTerrainImportResult Value;
    bool SetValue(const FTerrainImportResult& In){return MaybeBroadcastSuccessfulCompletion(FTerrainImportResult(In),Value);}
};


USTRUCT(BlueprintType)
struct FTerrainGraphSnapshot
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FString DefinitionPath;
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FString Revision;
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") bool GraphEnabled=false;
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FTerrainGraphData Graph;
};
USTRUCT(BlueprintType)
struct FTerrainRasterInfo
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FString Id;
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FString Name;
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FString Format;
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") int32 RawSize=0;
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") int32 ByteCount=0;
};
/** One GIS requirement profile, or a profile file that could not be read. */
USTRUCT(BlueprintType)
struct FTerrainGISProfileSummary
{
    GENERATED_BODY()
    /** The profile id (its JSON file name without the extension), e.g. game_level. Pass it as Profile to AssessGIS or CreateLevelProject. */
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FString Id;
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FString Label;
    /** Requirement ids in the order the assessment reports them, e.g. still_water, rivers, glacier, forest. */
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") TArray<FString> Requirements;
    /** The JSON file the profile was read from. */
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FString Path;
    /** Empty for a usable profile. Otherwise why the file was skipped; fix the file and call ListGISProfiles again. */
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FString Error;
};

UCLASS(BlueprintType)
class TERRAINFORGETOOLSET_API UToolCallAsyncResultTerrainCatalog : public UToolCallAsyncResult
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") TArray<FTerrainNodeSpec> Value;
    bool SetValue(const TArray<FTerrainNodeSpec>& In){return MaybeBroadcastSuccessfulCompletion(TArray<FTerrainNodeSpec>(In),Value);}
};

UCLASS(BlueprintType)
class TERRAINFORGETOOLSET_API UToolCallAsyncResultTerrainPreview : public UToolCallAsyncResult
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FTerrainPreviewInfo Value;
    bool SetValue(const FTerrainPreviewInfo& In){return MaybeBroadcastSuccessfulCompletion(FTerrainPreviewInfo(In),Value);}
};

UCLASS(BlueprintType)
class TERRAINFORGETOOLSET_API UToolCallAsyncResultTerrainRaster : public UToolCallAsyncResult
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FTerrainRasterInfo Value;
    bool SetValue(const FTerrainRasterInfo& In){return MaybeBroadcastSuccessfulCompletion(FTerrainRasterInfo(In),Value);}
};

UCLASS(BlueprintType)
class TERRAINFORGETOOLSET_API UToolCallAsyncResultTerrainProject : public UToolCallAsyncResult
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FTerrainGraphSnapshot Value;
    bool SetValue(const FTerrainGraphSnapshot& In){return MaybeBroadcastSuccessfulCompletion(FTerrainGraphSnapshot(In),Value);}
};

UCLASS(BlueprintType)
class TERRAINFORGETOOLSET_API UToolCallAsyncResultTerrainGISDiscovery : public UToolCallAsyncResult
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") TArray<FTerrainGISDataset> Value;
    bool SetValue(const TArray<FTerrainGISDataset>& In){return MaybeBroadcastSuccessfulCompletion(TArray<FTerrainGISDataset>(In),Value);}
};

UCLASS(BlueprintType)
class TERRAINFORGETOOLSET_API UToolCallAsyncResultTerrainGISAssessment : public UToolCallAsyncResult
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FTerrainGISAssessment Value;
    bool SetValue(const FTerrainGISAssessment& In){return MaybeBroadcastSuccessfulCompletion(FTerrainGISAssessment(In),Value);}
};

UCLASS(BlueprintType)
class TERRAINFORGETOOLSET_API UToolCallAsyncResultTerrainGIS : public UToolCallAsyncResult
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly,Category="TerrainForge") FTerrainGISStatus Value;
    bool SetValue(const FTerrainGISStatus& In){return MaybeBroadcastSuccessfulCompletion(FTerrainGISStatus(In),Value);}
};

/** Terrain recipes, local generation and new-level import. No deletion or credential tools. */
UCLASS()
class TERRAINFORGETOOLSET_API UTerrainForgeToolset : public UToolsetDefinition
{
    GENERATED_BODY()
public:
    virtual FString GetToolsetVersion() const override;
    /** Discover saved terrain recipes before reading or editing one. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Recipes")
    static TArray<FString> ListDefinitions();
    /** Create an unsaved terrain recipe at an unused /Game/ package path. Optional example has two lake basins. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Recipes")
    static FString CreateDefinition(const FString& AssetPath,bool bExampleValley=true);
    /** List the TerrainForge presets (native-only first): id, name, description, biome preset, node count, world size and build resolution. Pass an id to Create Definition From Preset or Apply Preset. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Recipes")
    static TArray<FTerrainPresetInfo> ListPresets();
    /** Create an unsaved graph definition at an unused /Game/ package path from a preset: its node graph plus its world settings (catalog recipe, biome preset, the preset's own overrides). Call List Presets for ids, e.g. alpine-eroded; Save Definition to keep it. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Recipes")
    static FString CreateDefinitionFromPreset(const FString& AssetPath,const FString& PresetId);
    /** Replace an existing definition's graph and world settings with a preset, as one undoable step. Needs the current revision from Get Graph; refused while a build runs. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Recipes")
    static bool ApplyPreset(const FString& DefinitionPath,const FString& ExpectedRevision,const FString& PresetId);
    /** Read every typed recipe parameter. Coordinates are metres from centre, X east and Y south. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Recipes")
    static FTerrainRecipe GetRecipe(const FString& DefinitionPath);
    /** Replace the typed recipe. Outputs from changed inputs become stale; immutable takes remain available. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Recipes")
    static bool SetRecipe(const FString& DefinitionPath,const FTerrainRecipe& Recipe);
    /** Check recipe structure and landscape sizing before generation. Returns diagnostics; empty means structurally valid. Generate Terrain performs full numerical validation too. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Recipes")
    static TArray<FString> ValidateRecipe(const FTerrainRecipe& Recipe);
    /** Save only this recipe asset. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Recipes")
    static bool SaveDefinition(const FString& DefinitionPath);
    /** Build a terrain take asynchronously with the native engine inside the editor; no external process or paid service. ThroughPass is Base, Geology, Detail, Water or Maps. Passes whose inputs are unchanged are reused from the cache whatever bResume says. Compute follows the TerrainForge Native Engine setting (default Auto: the GPU where a node has a GPU kernel, with CPU fallback). Waits until the take's files are written and hashed. Inspect the preview before Import New Level. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Generation")
    static UToolCallAsyncResultTerrain* GenerateTerrain(const FString& DefinitionPath,const FString& ThroughPass=TEXT("Maps"),bool bResume=true);
    /** Inspect job progress, current take, preview path and stale passes. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Generation")
    static FTerrainStatus GetStatus(const FString& DefinitionPath);
    /** Cancel the running build; Get Status then reports cancelled. Stored takes are retained. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Generation")
    static bool CancelGeneration(const FString& DefinitionPath);
    /** Check the persisted job after an editor restart. A finished take is kept; a build the restart interrupted cannot be reconnected, so call Generate Terrain again. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Generation")
    static bool RecoverGeneration(const FString& DefinitionPath);
    /** List this recipe's immutable takes. No tool deletes vault data. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Generation")
    static TArray<FString> ListTakes(const FString& DefinitionPath);
    /** Import the current validated take into a NEW World Partition map. Does not replace existing content. Includes landscape, configurable weights, neutral material, water and optional Nanite. Interactive import supports up to 8129 samples when the memory guard finds enough free memory (Nanite up to 4033, larger takes import with Nanite off); 16321 takes use the headless TerrainForgeImport commandlet. Open map must be clean; inspect preview first. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Import")
    static UToolCallAsyncResultTerrainImport* ImportNewLevel(const FString& DefinitionPath,const FString& LevelPath=TEXT("/Game/TerrainForge/Test/L_TerrainForgeTest"),bool bNanite=true);
    /** Open the recipe editor; its controls call the same API as these tools. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Editor")
    static void OpenDefinition(const FString& DefinitionPath);
    /** Read the canonical saved graph and its revision together. Use this revision for graph mutations; refresh after conflicts. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static FTerrainGraphSnapshot GetGraph(const FString& DefinitionPath);
    /** List typed node ports, parameters, ranges and choices from the saved catalog. Call Refresh Node Catalog after updating TerrainForge. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static TArray<FTerrainNodeSpec> ListNodeTypes(const FString& DefinitionPath);
    /** Load the native node catalog into the definition. Calls the core graph subsystem; no editor UI is required. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static UToolCallAsyncResultTerrainCatalog* RefreshNodeCatalog(const FString& DefinitionPath);
    /** Validate graph structure, parameter types, connections, cycles and exports without applying it. Evaluation also checks required inputs. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static TArray<FString> ValidateGraph(const FString& DefinitionPath,const FTerrainGraphData& Graph);
    /** Atomically replace the typed graph, including output, water mode and named export ports. Revision mismatch preserves current edits. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static bool ReplaceGraph(const FString& DefinitionPath,const FString& ExpectedRevision,const FTerrainGraphData& Graph);
    /** Add a catalog node with its real parameter defaults. Position is editor layout; feature coordinates are node parameters in metres. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static FTerrainGraphNodeData AddNode(const FString& DefinitionPath,const FString& ExpectedRevision,const FString& Type,FVector2D Position);
    /** Replace one node including typed parameters, links and bypass. The id and type must already exist. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static bool SetNode(const FString& DefinitionPath,const FString& ExpectedRevision,const FTerrainGraphNodeData& Node);
    /** Remove a node and downstream links. Reassign final/output exports before removing their source node. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static bool RemoveNode(const FString& DefinitionPath,const FString& ExpectedRevision,const FString& NodeId);
    /** Connect one typed output to an input. Rejects cycles; replaces the previous link on that input. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static bool ConnectNodes(const FString& DefinitionPath,const FString& ExpectedRevision,const FString& TargetNode,const FString& Input,const FTerrainGraphLink& Source);
    /** Disconnect an input without deleting either node. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static bool DisconnectNodes(const FString& DefinitionPath,const FString& ExpectedRevision,const FString& TargetNode,const FString& Input);
    /** Choose the node whose terrain output becomes the final terrain. Named mask and color exports remain independent. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static bool SetGraphOutput(const FString& DefinitionPath,const FString& ExpectedRevision,const FString& NodeId);
    /** Evaluate a node output asynchronously at 33..1025 samples; returns the original preview field file. Use Generate Terrain and recipe SimulationResolution 2017/4033 for final native builds. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Graph")
    static UToolCallAsyncResultTerrainPreview* PreviewNode(const FString& DefinitionPath,const FString& NodeId,const FString& Output=TEXT("terrain"),int32 Resolution=513);
    /** List embedded raster metadata without returning the binary source data. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Sources")
    static TArray<FTerrainRasterInfo> ListSources(const FString& DefinitionPath);
    /** Embed a PNG/TIFF/RAW/R16 file in the definition. RawSize is required for headerless files. Assign returned id to a HeightfieldFile or MaskFile node. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Sources")
    static UToolCallAsyncResultTerrainRaster* ImportRaster(const FString& DefinitionPath,const FString& Filename,int32 RawSize=0);
    /** Replace the graph/world with a portable TerrainForge JSON project, verifying its embedded sources. Concurrent edits cause rejection. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Sources")
    static UToolCallAsyncResultTerrainProject* ImportProject(const FString& DefinitionPath,const FString& Filename);
    /** Save a portable JSON project with all referenced raster bytes. Does not replace or save the Unreal definition asset. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Sources")
    static bool SaveProject(const FString& DefinitionPath,const FString& Filename);
    /** Discover supported data-provider datasets for a WGS84 location and extent. Runs natively and needs internet access to the providers. Availability is provider-specific; unmapped does not mean absent in reality. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|GIS")
    static UToolCallAsyncResultTerrainGISDiscovery* DiscoverGIS(const FTerrainGISSelection& Selection);
    /** Import selected elevation/vector masks asynchronously into the same asset. False adds source nodes and requires matching physical extent; true replaces graph/world. Original elevation is not eroded. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|GIS")
    static UToolCallAsyncResultTerrainGIS* ImportGIS(const FString& DefinitionPath,const FTerrainGISSelection& Selection,bool bReplaceGraph=false);
    /** Check whether a WGS84 square can become a level from real data, against a requirement profile (game_level ships with the plugin; a project can add its own; List GIS Profiles gives the ids): elevation resolution and coverage (bare earth or surface model), still water verified level against the DEM, rivers, glaciers, forest, grassland, rock, scree, wetland, paths, roads, buildings, treeline/snowline and licences. Each requirement is native, partial, absent or missing; missing ones carry the generated fill a level build applies. Verdict: ready, ready_with_fills or not_recommended. Proceed with CreateLevelProject. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|GIS")
    static UToolCallAsyncResultTerrainGISAssessment* AssessGIS(const FTerrainGISSelection& Selection,const FString& Profile=TEXT("game_level"));
    /** Proceed after AssessGIS: build a level project on a 1 m grid (Selection.Resolution 2017 or 4033; the square is resolution - 1 metres) from the assessed sources and generated fills, then replace the definition's graph, sources and world settings. Profile is game_level or a project profile id from ListGISProfiles; use the one you assessed with. Poll GetGISStatus; undo restores the previous graph. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|GIS")
    static UToolCallAsyncResultTerrainGIS* CreateLevelProject(const FString& DefinitionPath,const FTerrainGISSelection& Selection,const FString& Profile=TEXT("game_level"));
    /** List the GIS requirement profiles AssessGIS and CreateLevelProject accept: id, label and requirement ids. game_level ships with the plugin; JSON files in the project's Config/TerrainForge/GISProfiles folder or in the folders of the TerrainForge GIS settings add profiles or replace one with the same id. Files that could not be read are listed last with an Error. No side effects. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|GIS")
    static TArray<FTerrainGISProfileSummary> ListGISProfiles();
    /** Read persisted GIS progress independently of terrain generation. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|GIS")
    static FTerrainGISStatus GetGISStatus(const FString& DefinitionPath);
    /** Request GIS cancellation. Downloaded source caches and completed projects are retained. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|GIS")
    static void CancelGIS(const FString& DefinitionPath);
    /** List exact filenames available for output bindings in the selected take. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Export")
    static TArray<FString> ListArtifacts(const FString& DefinitionPath);
    /** Read saved source-to-semantic assignments for Landscape layers, PCG masks, textures, water geometry and GIS vectors. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Export")
    static FTerrainExportSettings GetExportSettings(const FString& DefinitionPath);
    /** Save typed output bindings using the current definition revision. Does not generate, export or modify a level. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Export")
    static bool SetExportSettings(const FString& DefinitionPath,const FString& ExpectedRevision,const FTerrainExportSettings& Settings);
    /** Validate selected take, mask dimensions, semantic names and exact normalization of weight-blended Landscape layers. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Export")
    static TArray<FString> ValidateExport(const FString& DefinitionPath);
    /** Create a new saved Terrain Export Bundle, textures/layer infos and copied height/water/GIS handoff files. Existing content is preserved; no level is switched. */
    UFUNCTION(meta=(AICallable),Category="TerrainForge|Export")
    static FTerrainExportResult ExportAssets(const FString& DefinitionPath);
};
