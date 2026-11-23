<!-- kovati:header -->
<p align="center"><a href="https://kovati.dev/tools/terrainforge/"><img src="https://raw.githubusercontent.com/AutomationForgeHQ/.github/main/profile/assets/terrainforge.jpg" width="100%" alt="TerrainForge, part of Kovati" /></a></p>

<p align="center"><b>Agent toolset for TerrainForge</b> · Part of <a href="https://kovati.dev"><b>Kovati</b></a>, production tools for Unreal games · <a href="https://kovati.dev/worldbuilding/">Worldbuilding</a><br />
<a href="https://kovati.dev/tools/terrainforge/">Product page</a> · <a href="https://docs.kovati.dev">Documentation</a> · <a href="https://kovati.dev/download/">Install with the Kovati Hub</a> · <a href="https://kovati.dev/discord">Discord</a></p>
<!-- /kovati:header -->

# TerrainForgeToolset

<!-- forge:version -->**Version 0.1.1. Preview.**<!-- /forge:version --> Free and open source, Apache-2.0.

Agent access to TerrainForge through Unreal Engine 5.8's toolset registry. An MCP client can build a terrain the way
a person does in the editor: start from a preset, edit the graph, preview nodes, build, bring in real-world data,
make a level project, export, and import the result as a new level.

The toolset is an adapter and nothing more. Every tool calls the same TerrainForge subsystems the editor's buttons
call, so an agent and a person can work on the same definition.

## Requirements

- Unreal Engine 5.8, editor only.
- **TerrainForge 0.2.0 or later**, the plugin this toolset drives. TerrainForge is a paid product with a free Lite
  tier, installed through the Kovati Hub. The toolset has no tiers of its own: in Lite, TerrainForge refuses a build,
  import or export over 1009 x 1009 samples or 2 km x 2 km and runs every node on the CPU, and the tools pass on its
  reason. See TerrainForge's README for Lite and Full.
- The engine's **ToolsetRegistry** plugin, which this plugin enables. To reach the tools from an MCP client, also
  enable the engine's **Model Context Protocol** plugin.
- Internet access only for the GIS tools.

## The tools

<!-- forge:tools -->44 tools<!-- /forge:tools --> in toolset `TerrainForgeToolset.TerrainForgeToolset`. Each row is taken from the
function's own doc comment, which is also the description an agent reads.

| Area | Tool | What it does |
| --- | --- | --- |
| Definitions | `ListDefinitions` | Find saved terrain definitions. |
| | `CreateDefinition` | Create an unsaved definition at an unused `/Game/` path, optionally with a two-basin example. |
| | `ListPresets` | List the presets: id, name, description, biome, node count, world size and resolution. |
| | `CreateDefinitionFromPreset` | Create an unsaved definition from a preset's graph and world settings. |
| | `ApplyPreset` | Replace a definition's graph and world settings with a preset, as one undoable step. |
| | `GetRecipe` / `SetRecipe` | Read or replace the typed world settings. |
| | `ValidateRecipe` | Check structure and landscape sizing before a build. |
| | `SaveDefinition` | Save only this definition. |
| | `OpenDefinition` | Open the definition in the TerrainForge editor. |
| Graph | `GetGraph` | Read the graph and its revision. Every edit below needs that revision. |
| | `ListNodeTypes` | List node ports, parameters, ranges and choices. |
| | `RefreshNodeCatalog` | Load the node catalog into the definition. |
| | `ValidateGraph` | Check a graph's structure, types, links, cycles and exports without applying it. |
| | `ReplaceGraph` | Replace the whole graph in one step. |
| | `AddNode` / `SetNode` / `RemoveNode` | Add a node with its real defaults, replace one, or remove one. |
| | `ConnectNodes` / `DisconnectNodes` | Link an output to an input, or unlink an input. Cycles are refused. |
| | `SetGraphOutput` | Choose the node whose terrain becomes the final terrain. |
| | `PreviewNode` | Evaluate one node output at 33 to 1025 samples. |
| Sources | `ListSources` | List the embedded rasters, without their bytes. |
| | `ImportRaster` | Embed a PNG, TIFF, RAW or R16 file for a heightfield or mask node. |
| | `ImportProject` / `SaveProject` | Read or write a portable JSON project with its sources. |
| Builds | `GenerateTerrain` | Build a take through a chosen pass. Unchanged passes are reused. |
| | `GetStatus` | Progress, the current take, its preview and stale passes. |
| | `CancelGeneration` | Stop the running build. Finished takes are kept. |
| | `RecoverGeneration` | After an editor restart, keep a finished take or clean up an interrupted build. |
| | `ListTakes` | List a definition's takes. |
| GIS | `DiscoverGIS` | List the elevation, land-cover and OpenStreetMap data covering a WGS84 square. |
| | `ImportGIS` | Import elevation and up to 15 masks, replacing the graph or adding to it. |
| | `ListGISProfiles` | List the requirement profiles: the shipped `game_level` and the project's own. |
| | `AssessGIS` | Check a square against a profile. Verdict: ready, ready with fills, or not recommended. |
| | `CreateLevelProject` | Build a 1 m level project at 2017 or 4033 samples from the assessed data and fills. |
| | `GetGISStatus` / `CancelGIS` | Read a GIS job's progress, or cancel it. Downloaded data is kept. |
| Export | `ListArtifacts` | List the files in the selected take. |
| | `GetExportSettings` / `SetExportSettings` | Read or save the bindings to Landscape layers, PCG masks, textures, water and GIS vectors. |
| | `ValidateExport` | Check the take, mask sizes, names and the 255 sum of weight-blended layers. |
| | `ExportAssets` | Write a new Terrain Export Bundle with its textures, layer infos and hand-off files. |
| Level | `ImportNewLevel` | Import the current take into a new World Partition map. |

`GenerateTerrain`, `ImportNewLevel`, `PreviewNode`, `RefreshNodeCatalog`, `ImportRaster`, `ImportProject`,
`DiscoverGIS`, `ImportGIS`, `AssessGIS` and `CreateLevelProject` return typed asynchronous results. Every other tool
answers at once. Parameters and results are reflected structs, so the schema an agent reads is the signature.

Builds follow the TerrainForge Native Engine settings. The compute default is **Auto**: nodes with a GPU kernel run
on the GPU, with a CPU fallback, and everything else on the CPU. In TerrainForge Lite every node runs on the CPU.

`ImportNewLevel` takes up to 8129 samples, with Nanite up to 4033, in TerrainForge Full (1009 in Lite). Larger takes
import through TerrainForge's headless `TerrainForgeImport` commandlet.

## Deliberately absent

- **No tool deletes.** Takes are immutable, and no tool removes a definition, a take or a map.
- **No tool overwrites a level.** `ImportNewLevel` refuses an existing map path, and `ExportAssets` creates a new
  bundle and leaves existing content in place.
- **No tool writes a credential.** Map-layer keys are entered by a person in the editor settings.

## The skills

Eight native agent skills ship with the toolset and are discovered automatically. Each description says when to load
it: authoring a layout from a description, generation, import into a level, water, troubleshooting, graph editing,
GIS imports, and export bindings.

## A first session

A typical order of calls, in any MCP client connected to the editor:

1. `ListPresets`, then `CreateDefinitionFromPreset` with an unused `/Game/` path and a preset id.
2. `GetGraph` for the revision, then graph edits and `PreviewNode` on the nodes you change.
3. `GenerateTerrain` with `throughPass` `Maps`. Poll `GetStatus` and open its preview image.
4. `ImportNewLevel` with an unused level path.

For real terrain, call `DiscoverGIS` and `ImportGIS` for a square, or `AssessGIS` followed by `CreateLevelProject` for
a level project.

## Licence

Apache-2.0, in [LICENSE](LICENSE). TerrainForge itself is a paid product, with a free Lite tier, under its own
licence.
