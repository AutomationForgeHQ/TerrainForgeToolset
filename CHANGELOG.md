# TerrainForgeToolset

Every released version of TerrainForgeToolset, newest first. A release publishes **one** section of this
file — the one whose heading matches its tag — as its release notes. Write for someone who
installs the plugin, not for the commit log.

Headings are `## <x.y.z> — <date>`. Use `Added` / `Changed` / `Fixed` / `Compatibility` /
`Known issues`, only the ones that apply. A version that has not shipped says `unreleased`
in place of the date; set the date when it is tagged.

## 0.1.0 — 2026-10-29

**The first release**, alongside the TerrainForge 0.1.0 preview. Agent access to TerrainForge through Unreal Engine
5.8's toolset registry and MCP.

### Added
- **44 typed tools** over the same API the TerrainForge editor uses: definitions and presets, graph editing with
  revision checks, node previews, raster and project import, builds, GIS discovery and import, requirement profiles,
  location checks, level projects, export bindings and import into a new level.
- **Asynchronous results** for the long calls (builds, previews, imports, GIS jobs), typed like every other result.
- **Eight native agent skills**: authoring, generation, import, water, troubleshooting, graph editing, GIS and
  export. They are discovered automatically.
- **Safe by design.** No tool deletes a definition, a take or a map, overwrites a level, or writes a credential.

### Compatibility
- **Unreal Engine 5.8, editor only.**
- **Needs TerrainForge 0.1.0** and the engine's ToolsetRegistry plugin. An MCP client connects through the engine's
  Model Context Protocol plugin.
- **The GIS tools need internet access** to the data providers. Every other tool works offline.
