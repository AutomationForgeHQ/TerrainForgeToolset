#pragma once
#include "ToolsetRegistry/AgentSkill.h"
#include "TerrainForgeSkills.generated.h"

UCLASS()
class UTerrainForgeAuthoringSkill : public UAgentSkill
{
    GENERATED_BODY()
public:
    UTerrainForgeAuthoringSkill()
    {
        Description=TEXT("Load when translating a terrain description into an authored TerrainForge layout.");
        Instructions = TEXT(
            "Work in metres around the map centre: east is positive X, south is positive Y. "
            "Give ranges several points; a peak is a single point. "
            "Range heights add relief, while a plateau or valley height is an absolute target altitude. "
            "Basins subtract depth from the surrounding land. Keep room in the vertical range for overlapping ranges. "
            "Start from the two-basin example and a low simulation resolution before increasing detail. "
            "To start from a tuned graph instead, call List Presets, then Create Definition From Preset (or Apply Preset on an existing definition); alpine-eroded is the native alpine preset (4 km at 4033) and alpine-eroded-8k its 8 km layout at 8129; both need TerrainForge Full, and in Lite the classic presets open as the same terrain scaled to 2 km. "
            "Applying a preset replaces the graph and recipe, keeps embedded sources and clears GIS provenance. "
            "Realism nodes: Geology builds a layered rock volume that a downstream Erode samples; Erode grade (0 keeps old graphs, 0.5 for new work) smooths pocked valley floors; "
            "Scree execution parallel is the fast mode and headTaper softens the straight head line of fresh aprons; Gullies adds fine incision in the gully band; "
            "Glaciate elaMode relief places the equilibrium line at elaFraction of the relief instead of an absolute ela; Sea levelMode coverage floods levelFraction of the map instead of an absolute seaLevel. "
            "Recipe treelineFraction above 0 places the treeline within the map's relief; 0 keeps the snowline minus gap rule. "
            "Biome rules produce both normalized coverage and independent placement masks. "
            "Named graph mask exports may guide those rules. "
            "Output assignments define Landscape layer names and downstream semantics.");
    }
};
UCLASS()
class UTerrainForgeGenerationSkill : public UAgentSkill
{
    GENERATED_BODY()
public:
    UTerrainForgeGenerationSkill()
    {
        Description=TEXT("Load before generating or resuming TerrainForge passes.");
        Instructions = TEXT(
            "Previews and take builds run natively inside the Unreal editor; no external process needs to be running and no paid service is used. "
            "Every node in the catalog runs natively; a graph that cannot be evaluated fails with the reason, so fix the graph instead of retrying. "
            "A definition without a graph builds once it is converted to a graph (Convert to graph in the definition editor); the converted terrain can look different from the old recipe result. "
            "Preview resolution, simulation resolution and exported Landscape resolution are separate controls. "
            "The Native 2K and Native 4K quality choices mean full resolution, 2017 or 4033 samples for both simulation and export; they are unrelated to the compute device. "
            "Upsampling a small simulation does not create simulated detail. "
            "The native viewer states its separate geometry cap; raise it explicitly to inspect full geometry. "
            "Small detail precedes the last drainage solve, so water and ground agree. "
            "Graph definitions evaluate their connected operations, not the legacy geological pass controls. "
            "Each run creates an immutable take. A pass whose inputs are unchanged is reused from the intermediate cache whatever device made it; the Maps pass always reruns. "
            "The Compute setting of the TerrainForge Native Engine defaults to Auto: nodes with a GPU kernel (Erode including Geology, Scree parallel, Hydraulic parallel droplets, Weathering droplet stages, blur, flow accumulation) run on the GPU when one is available and the grid is large enough to gain, everything else on the CPU. "
            "CPU Only keeps every node on the CPU, Prefer GPU uses the GPU at every size, and Reference runs each kernel's most precise CPU mode. "
            "A GPU kernel that cannot run falls back to the CPU automatically, and the node diagnostics and the Build monitor say which device ran; a node's compute param can still force the CPU. "
            "Measured on one high-end laptop GPU, a full 4033 take of the alpine preset took about 79 s with Auto against 233 s with CPU Only, and Erode and Hydraulic at 4033 ran about 7 to 10 times faster on the GPU; other cards differ, so quote these as measurements, not promises. "
            "Native builds run inside the editor; they stop if the editor closes and cannot be reconnected, so start them again. "
            "A changed recipe makes previous results stale. "
            "Save the recipe after generation to keep the selected take across editor restarts. "
            "TerrainForge runs in one of two tiers. Without a licence it is Lite: a build, Import New Level or Export Assets of more than 1009 x 1009 samples or of a world wider than 2 km is refused with one sentence (never downscaled), a preview above 1009 is computed at 1009, and every node runs on the CPU whatever the Compute setting or a node's compute param say. "
            "With a licence (Full) nothing is limited. Validate Recipe reports the Lite sentence; Set Recipe still accepts the recipe, so bring Resolution and SizeM under the limits and build again.");
    }
};
UCLASS()
class UTerrainForgeImportSkill : public UAgentSkill
{
    GENERATED_BODY()
public:
    UTerrainForgeImportSkill()
    {
        Description=TEXT("Load when importing generated TerrainForge terrain into a new Unreal level.");
        Instructions = TEXT(
            "Inspect the hillshade with water overlay before import. "
            "Use a new map under /Game/TerrainForge for experiments; never overwrite existing maps. "
            "The importer makes World Partition landscape, weight-blended layers, a neutral material, water and inspection lighting. "
            "Interactive import goes up to 8129 samples with Nanite up to 4033 (larger maps import with Nanite off); a memory guard refuses an import that will not fit, and 16321 imports run headless through the commandlet. "
            "The existing map must be clean and PIE stopped. "
            "Terrain weights are exactly 255 per pixel; a wrong size or 8-bit heightmap is rejected before creating the map. "
            "World Partition/Nanite saves can be expensive; inspect loaded regions before any later save. "
            "Capture both overhead and eye-level views after import.");
    }
};
UCLASS()
class UTerrainForgeWaterSkill : public UAgentSkill
{
    GENERATED_BODY()
public:
    UTerrainForgeWaterSkill()
    {
        Description=TEXT("Load when inspecting TerrainForge rivers, lake junctions or water artifacts.");
        Instructions = TEXT(
            "Drainage comes from the evolved terrain. "
            "Lake outlines have positive signed area in Unreal XY, and river points run from source to mouth with decreasing elevation. "
            "Each reach terminates at another reach, a lake or the boundary. "
            "Beds are already carved; water actors must not add a landscape brush or a Water edit layer. "
            "Factory-created actors have the engine water materials. "
            "Inspect narrow rivers close to the camera because the zone uses local tessellation. "
            "Large drops are listed as waterfall candidates, not simulated waterfalls. "
            "A validated centreline is still an approximation at the simulation grid resolution; inspect it in the landscape before dressing a level.");
    }
};
UCLASS()
class UTerrainForgeTroubleshootingSkill : public UAgentSkill
{
    GENERATED_BODY()
public:
    UTerrainForgeTroubleshootingSkill()
    {
        Description=TEXT("Load for failed TerrainForge jobs, stale passes or import problems.");
        Instructions = TEXT(
            "Start with Get Status and the definition editor's Build monitor tab, which shows each node's state, device, memory and warnings. "
            "A vertical-range error is a recipe problem: widen the encoding range instead of clipping peaks. "
            "A take that failed its validation or hash check is a failed take; do not import it. "
            "Flat terraces suggest an 8-bit source or too little vertical precision. "
            "Invisible lakes suggest reversed outlines; grey ground after a reload suggests missing landscape material instances or weight blending. "
            "The neutral material is for inspection; it does not provide production displacement. "
            "An error saying the native engine or TerrainForge GIS is not loaded means a module of the TerrainForge plugin is disabled or failed to start (see the Output Log), not that the definition is broken. "
            "GIS calls need internet access to the data providers they list; a provider error names the provider, and another source or a later retry usually works. "
            "Never force-close the editor to resolve a blocked import.");
    }
};

UCLASS()
class UTerrainForgeGraphSkill : public UAgentSkill
{
    GENERATED_BODY()
public:
    UTerrainForgeGraphSkill()
    {
        Description=TEXT("Load for native TerrainForge graph authoring shared with human editors.");
        Instructions = TEXT(
            "The Terrain Definition is canonical; the editor canvas is a transient drawing of it. "
            "Read a graph snapshot and its revision before editing; after any mutation or conflict, reread instead of overwriting another author's changes. "
            "Catalog parameter values are tagged Number, Text, Boolean, Numbers or Points; only the corresponding member is meaningful. "
            "Canvas Position is pixels, while feature points and widths are terrain-local metres. "
            "Ports distinguish terrain, mask, display color and linear RGB; they are not interchangeable. "
            "Disconnected draft nodes are allowed but required inputs must be connected before evaluating that branch. "
            "Select the final terrain output separately from named mask/color exports. "
            "Authored water mode exports the graph's rivers/lakes; drainage mode derives water from drainage; none suppresses water. "
            "An imported raster id references bytes embedded in the same asset, not an external path. "
            "Save the Unreal definition to persist editing; portable JSON is a separate interchange file. "
            "Previews and graph builds are computed natively in Unreal, on the GPU for nodes with a GPU kernel when the Compute setting allows it and on the CPU otherwise. "
            "GPU kernels are tested against their CPU counterparts, so a graph keeps its look when the device changes; the CPU is the reference when results must match exactly.");
    }
};
UCLASS()
class UTerrainForgeGISSkill : public UAgentSkill
{
    GENERATED_BODY()
public:
    UTerrainForgeGISSkill()
    {
        Description=TEXT("Load for georeferenced elevation, OSM vector and mask imports into TerrainForge definitions.");
        Instructions = TEXT(
            "Coordinates are WGS84 latitude/longitude; a local azimuthal-equidistant projection defines the square in metres. "
            "Row zero is north, local X east and Y south. "
            "Supported centres are within 80 degrees latitude; the Mercator source cannot cover polar terrain. "
            "Discovery, assessment, imports and level builds run natively inside the editor and need internet access to the data providers they list: national and global elevation services, ESA WorldCover land cover and the OpenStreetMap Overpass API, whose endpoint can be changed in the TerrainForge GIS settings. "
            "Discover supported datasets before choosing them. "
            "To decide whether a place can become a level, call AssessGIS first: it reports per requirement native, partial, absent or missing data, the generated fill for each gap and a verdict; then CreateLevelProject with the same profile builds and adopts the project (1 m grid, 1009, 2017 or 4033 samples; TerrainForge Lite builds 1009, a 1008 m level). "
            "Both follow a requirement profile: game_level ships with the plugin and paints Snow, Rocks, Scree, Meadow, Wetland, Soil, Forest and Water; a project adds or overrides profiles with JSON files, and ListGISProfiles lists the ids, their requirements and any file that failed to load. "
            "Planned tile coverage is not proof of elevation survey precision; source spacing, date and vertical datum vary. "
            "Raster resampling adds no measured detail. OpenStreetMap is incomplete: unmapped is different from absent. "
            "Water/forest/road/building observations become vector geometry and masks; approximate line widths are recorded where real widths are missing. "
            "They do not automatically carve river beds or simulate water stages. "
            "Preserve provider attribution and projection records in exported work; OpenStreetMap-derived vectors and masks carry the ODbL attribution. "
            "Merging sources requires matching physical extents; no automatic alignment with a previously georeferenced terrain is inferred. "
            "Replacement is explicit and may change world height range and graph. "
            "Imports have independent asynchronous progress and cancellation, and one import or level build runs at a time; concurrent definition edits prevent adoption of an obsolete result.");
    }
};
UCLASS()
class UTerrainForgeExportSkill : public UAgentSkill
{
    GENERATED_BODY()
public:
    UTerrainForgeExportSkill()
    {
        Description=TEXT("Load when binding TerrainForge outputs to Unreal Landscape layers or downstream PCG tools.");
        Instructions = TEXT(
            "Generate a current non-stale take through Maps before export. "
            "Bind exact source artifact basenames to unique semantic target names. biome_<name> maps are normalized coverage; pcg_<name> maps are independent suitability and can overlap. "
            "PCG density is not an instance count per square metre. "
            "Weight-blended Landscape bindings must collectively sum to 255 at every sample; use independent alpha layers for a subset or overlapping masks. "
            "Keep data textures linear, sample categorical ids with nearest filtering, and preserve row orientation. "
            "Height encoding is min+(uint16/65536)*(max-min), not division by 65535. "
            "The saved Terrain Export Bundle carries original resolution, extent, transform, named textures/layer infos and file locations. "
            "It is an editor handoff asset; standard exported texture/layer assets can be cooked. "
            "Water surface/bottom OBJ coordinates are metres with X east,Y south,Z up; multiply by 100 for Unreal centimetres. "
            "Waterlines preserve physical boundary elevation. "
            "GIS masks are observations, not evidence of a simulated water volume. "
            "Exporting assets does not create a landscape or switch levels. "
            "Interactive new-level import caps at 8129 (Nanite at 4033) and refuses a take whose estimated memory does not fit; 16321 imports headless; "
            "4033 generation/export is separate from validating a 4K landscape import.");
    }
};
