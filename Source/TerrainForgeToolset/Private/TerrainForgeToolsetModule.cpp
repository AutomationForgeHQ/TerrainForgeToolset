#include "TerrainForgeToolset.h"
#include "ToolsetRegistry/UToolsetRegistry.h"
#include "Modules/ModuleManager.h"
class FTerrainForgeToolsetModule : public IModuleInterface
{
public:
    virtual void StartupModule() override{if(UToolsetRegistry::IsAvailable())UToolsetRegistry::RegisterToolsetClass(UTerrainForgeToolset::StaticClass());}
    virtual void ShutdownModule() override{if(UToolsetRegistry::IsAvailable())UToolsetRegistry::UnregisterToolsetClass(UTerrainForgeToolset::StaticClass());}
};
IMPLEMENT_MODULE(FTerrainForgeToolsetModule,TerrainForgeToolset)
