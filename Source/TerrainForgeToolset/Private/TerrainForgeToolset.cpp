#include "TerrainForgeToolset.h"
#include "TerrainForgeSubsystem.h"
#include "TerrainForgeGraphSubsystem.h"
#include "Editor.h"
#include "UObject/StrongObjectPtr.h"

namespace
{
    UTerrainForgeSubsystem* API(){return GEditor?GEditor->GetEditorSubsystem<UTerrainForgeSubsystem>():nullptr;}
    UTerrainDef* Definition(const FString& Path)
    {auto* Def=LoadObject<UTerrainDef>(nullptr,*Path);if(!Def)UTerrainForgeSubsystem::Fail(TEXT("Terrain recipe not found. Call List Definitions or Create Definition."));return Def;}
}
TArray<FString> UTerrainForgeToolset::ListDefinitions(){return API()->ListDefinitions();}
FString UTerrainForgeToolset::CreateDefinition(const FString& Path,bool bExampleValley)
{auto* Def=API()->CreateDefinition(Path,bExampleValley);return Def?Def->GetPathName():FString();}
TArray<FTerrainPresetInfo> UTerrainForgeToolset::ListPresets()
{FString Error;auto Presets=UTerrainForgePresetLibrary::ListPresets(Error);if(Presets.IsEmpty())UTerrainForgeSubsystem::Fail(Error.IsEmpty()?FString(TEXT("No presets available. Enable the TerrainForge Native module.")):Error);return Presets;}
FString UTerrainForgeToolset::CreateDefinitionFromPreset(const FString& Path,const FString& PresetId)
{
    // Check first, so an unknown id, an unreadable preset or one this installation cannot open creates no asset.
    FString Error;
    if(!UTerrainForgePresetLibrary::CheckPreset(PresetId,Error)){UTerrainForgeSubsystem::Fail(Error);return FString();}
    auto* Def=API()?API()->CreateDefinition(Path,false):nullptr;if(!Def)return FString();
    if(!UTerrainForgePresetLibrary::ApplyPreset(Def,PresetId,false,Error))
    {UTerrainForgeSubsystem::Fail(Error+TEXT(" The new definition keeps the starter graph; call Apply Preset after fixing the cause."));return FString();}
    return Def->GetPathName();
}
bool UTerrainForgeToolset::ApplyPreset(const FString& Path,const FString& ExpectedRevision,const FString& PresetId)
{
    auto* Def=Definition(Path);if(!Def)return false;
    if(ExpectedRevision.IsEmpty()||ExpectedRevision!=UTerrainForgeGraphSubsystem::Revision(Def)){UTerrainForgeSubsystem::Fail(TEXT("Definition changed. Call Get Graph for the current revision, then apply the preset again."));return false;}
    FString Error;if(UTerrainForgePresetLibrary::ApplyPreset(Def,PresetId,true,Error))return true;
    UTerrainForgeSubsystem::Fail(Error);return false;
}
FTerrainRecipe UTerrainForgeToolset::GetRecipe(const FString& Path){return API()->GetRecipe(Definition(Path));}
bool UTerrainForgeToolset::SetRecipe(const FString& Path,const FTerrainRecipe& Recipe){return API()->SetRecipe(Definition(Path),Recipe);}
TArray<FString> UTerrainForgeToolset::ValidateRecipe(const FTerrainRecipe& Recipe){return API()->ValidateRecipe(Recipe);}
bool UTerrainForgeToolset::SaveDefinition(const FString& Path){return API()->SaveDefinition(Definition(Path));}
FTerrainStatus UTerrainForgeToolset::GetStatus(const FString& Path){return API()->GetStatus(Definition(Path));}
bool UTerrainForgeToolset::CancelGeneration(const FString& Path){return API()->CancelGeneration(Definition(Path));}
bool UTerrainForgeToolset::RecoverGeneration(const FString& Path){return API()->RecoverGeneration(Definition(Path));}
TArray<FString> UTerrainForgeToolset::ListTakes(const FString& Path){return API()->ListTakes(Definition(Path));}
void UTerrainForgeToolset::OpenDefinition(const FString& Path){API()->OpenDefinition(Definition(Path));}
UToolCallAsyncResultTerrain* UTerrainForgeToolset::GenerateTerrain(const FString& Path,const FString& ThroughPass,bool bResume)
{
    auto* Def=Definition(Path);auto* Result=NewObject<UToolCallAsyncResultTerrain>();
    if(!Def){Result->SetError(TEXT("Terrain recipe not found. Call List Definitions or Create Definition."));return Result;}
    if(!API()->StartGeneration(Def,ThroughPass,bResume))
    {
        // The core keeps the reason it refused where Get Status reads it; pass it on as it is.
        const FString Why=API()->GetStatus(Def).Error;
        Result->SetError(Why.IsEmpty()?FString(TEXT("Generation could not start. Call Validate Recipe and Validate Graph, then Get Status.")):Why);return Result;
    }
    Result->AddToRoot();auto Keep=MakeShared<TStrongObjectPtr<UTerrainDef>>(Def);double Started=FPlatformTime::Seconds();
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Result,Keep,Started](float)
    {
        if(!API()||FPlatformTime::Seconds()-Started>14500){Result->SetError(TEXT("Generation wait expired. Call Get Status."));Result->RemoveFromRoot();return false;}
        FTerrainStatus Status=API()->GetStatus(Keep->Get());
        if(Status.State==TEXT("failed")||Status.State==TEXT("cancelled"))
        {Result->SetError(Status.Error.IsEmpty()?TEXT("Generation cancelled. Call Generate Terrain to start a new take."):Status.Error);Result->RemoveFromRoot();return false;}
        if(Status.State==TEXT("succeeded")){Result->SetValue(Status);Result->RemoveFromRoot();return false;}
        return true;
    }),.25f);return Result;
}
UToolCallAsyncResultTerrainImport* UTerrainForgeToolset::ImportNewLevel(const FString& Path,const FString& LevelPath,bool bNanite)
{
    auto* Def=Definition(Path);auto* Result=NewObject<UToolCallAsyncResultTerrainImport>();
    if(!Def){Result->SetError(TEXT("Recipe not found. Call List Definitions."));return Result;}
    Result->AddToRoot();auto Keep=MakeShared<TStrongObjectPtr<UTerrainDef>>(Def);
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Result,Keep,LevelPath,bNanite](float)
    {
        auto Value=API()->ImportNewLevel(Keep->Get(),LevelPath,bNanite);
        // The core returns why an import failed; pass it on as it is.
        if(Value.LandscapePath.IsEmpty())Result->SetError(Value.Error.IsEmpty()?FString(TEXT("Import failed. Check recipe status, preview, unused level path and Output Log before Import New Level.")):Value.Error);
        else Result->SetValue(Value);
        Result->RemoveFromRoot();return false;
    }));return Result;
}
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "TerrainForgeModule.h"
#include "TerrainForgeSkills.h"
#include "ToolsetRegistry/UToolsetRegistry.h"
#include "UObject/UObjectIterator.h"

namespace TerrainForgeToolsetTest
{
    // Words of the retired external-process pipeline (its runner, its local ports 876x, its provider tools) and the stale
    // compute claims; none may reach an agent. The source-tree grep covers the names this file must not spell out.
    const TCHAR* const Retired[]={TEXT("876"),TEXT("Landlab"),TEXT("runner"),TEXT("Python"),TEXT("StartProvider"),TEXT("ListProviders"),TEXT("setup.ps1"),
        TEXT("default is CPU Only"),TEXT("not been measured"),TEXT("CPU-only")};
    TArray<UFunction*> Tools()
    {
        TArray<UFunction*> Out;
        for(TFieldIterator<UFunction> It(UTerrainForgeToolset::StaticClass(),EFieldIteratorFlags::ExcludeSuper);It;++It)if(It->HasMetaData(TEXT("AICallable")))Out.Add(*It);
        return Out;
    }
    TArray<UClass*> Skills()
    {
        TArray<UClass*> Out;
        for(TObjectIterator<UClass> It;It;++It)
            if(It->IsChildOf(UAgentSkill::StaticClass())&&!It->HasAnyClassFlags(CLASS_Abstract)&&It->GetOutermost()==UTerrainForgeToolset::StaticClass()->GetOutermost())Out.Add(*It);
        return Out;
    }
    /** Answers Profiles() only; the toolset reads nothing else from the service in these tests. */
    class FProfilesOnlyService final : public ITerrainGISService
    {
    public:
        TArray<FTerrainGISProfileInfo> Answer;
        virtual void Discover(const FTerrainGISSelection&,TFunction<void(const TArray<FTerrainGISDataset>&,const FString&)> Done) override{Done({},TEXT("not used"));}
        virtual void Assess(const FTerrainGISSelection&,const FString&,TFunction<void(const FTerrainGISAssessment&,const FString&)> Done) override{Done({},TEXT("not used"));}
        virtual FString StartImport(const FTerrainGISSelection&,FTerrainGISProgress,FTerrainGISPackageDone) override{return FString();}
        virtual FString StartLevelBuild(const FTerrainGISSelection&,const FString&,const FString&,FTerrainGISProgress,FTerrainGISPackageDone) override{return FString();}
        virtual bool Cancel(const FString&) override{return false;}
        virtual bool IsBusy() const override{return false;}
        virtual TArray<FTerrainCoverageSourceInfo> SourceCatalogue() const override{return {};}
        virtual TArray<FTerrainGISProfileInfo> Profiles() const override{return Answer;}
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTerrainForgeToolsetToolsTest,"TerrainForge.Toolset.Tools",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTerrainForgeToolsetToolsTest::RunTest(const FString&)
{
    using namespace TerrainForgeToolsetTest;
    TSet<FString> Names;
    for(UFunction* F:Tools())
    {
        Names.Add(F->GetName());
        TestTrue(FString::Printf(TEXT("%s is static"),*F->GetName()),F->HasAnyFunctionFlags(FUNC_Static));
        TestTrue(FString::Printf(TEXT("%s has a category"),*F->GetName()),F->GetMetaData(TEXT("Category")).StartsWith(TEXT("TerrainForge|")));
        const FString Tip=F->GetMetaData(TEXT("ToolTip"));
        TestFalse(FString::Printf(TEXT("%s has a description"),*F->GetName()),Tip.IsEmpty());
        for(const TCHAR* Word:Retired)TestFalse(FString::Printf(TEXT("%s description names '%s'"),*F->GetName(),Word),Tip.Contains(Word));
    }
    AddInfo(FString::Printf(TEXT("%d tools: %s"),Names.Num(),*FString::Join(Names.Array(),TEXT(", "))));
    TestEqual(TEXT("tool count (45 before, minus StartProvider and ListProviders, plus ListGISProfiles)"),Names.Num(),44);
    TestFalse(TEXT("StartProvider is gone"),Names.Contains(TEXT("StartProvider")));
    TestFalse(TEXT("ListProviders is gone"),Names.Contains(TEXT("ListProviders")));
    TestTrue(TEXT("ListGISProfiles is a tool"),Names.Contains(TEXT("ListGISProfiles")));
    for(const TCHAR* Kept:{TEXT("DiscoverGIS"),TEXT("AssessGIS"),TEXT("CreateLevelProject"),TEXT("ImportGIS"),TEXT("GetGISStatus"),TEXT("CancelGIS"),TEXT("GenerateTerrain")})
        TestTrue(FString::Printf(TEXT("%s is kept"),Kept),Names.Contains(Kept));
    // The schema the registry publishes is the one an agent reads.
    if(UToolsetRegistry::IsAvailable())
    {
        const FString Schema=UToolsetRegistry::GetToolsetJsonSchema(UTerrainForgeToolset::StaticClass());
        TestTrue(TEXT("schema lists ListGISProfiles"),Schema.Contains(TEXT("ListGISProfiles")));
        TestFalse(TEXT("schema has no StartProvider"),Schema.Contains(TEXT("StartProvider")));
        TestFalse(TEXT("schema has no ListProviders"),Schema.Contains(TEXT("ListProviders")));
        // Only names here: the schema also carries the comments of TerrainForge's own types, which are not this plugin's text.
    }
    else AddWarning(TEXT("ToolsetRegistry is not available; the published schema was not checked."));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTerrainForgeToolsetSkillsTest,"TerrainForge.Toolset.Skills",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTerrainForgeToolsetSkillsTest::RunTest(const FString&)
{
    using namespace TerrainForgeToolsetTest;
    const TArray<UClass*> Classes=Skills();
    TestEqual(TEXT("eight skills"),Classes.Num(),8);
    for(UClass* C:Classes)
    {
        const UAgentSkill* Skill=GetDefault<UAgentSkill>(C);
        const FString Text=Skill->Description+TEXT("\n")+Skill->GetDetails().Instructions;
        TestFalse(FString::Printf(TEXT("%s has instructions"),*C->GetName()),Skill->GetDetails().Instructions.IsEmpty());
        for(const TCHAR* Word:Retired)TestFalse(FString::Printf(TEXT("%s names '%s'"),*C->GetName(),Word),Text.Contains(Word));
    }
    const FString Generation=GetDefault<UAgentSkill>(UTerrainForgeGenerationSkill::StaticClass())->GetDetails().Instructions;
    TestTrue(TEXT("Generation names Auto as the default"),Generation.Contains(TEXT("defaults to Auto")));
    const FString GIS=GetDefault<UAgentSkill>(UTerrainForgeGISSkill::StaticClass())->GetDetails().Instructions;
    TestTrue(TEXT("GIS names game_level"),GIS.Contains(TEXT("game_level")));
    TestTrue(TEXT("GIS names CreateLevelProject"),GIS.Contains(TEXT("CreateLevelProject")));
    TestTrue(TEXT("GIS names ListGISProfiles"),GIS.Contains(TEXT("ListGISProfiles")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTerrainForgeToolsetGISProfilesTest,"TerrainForge.Toolset.GISProfiles",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTerrainForgeToolsetGISProfilesTest::RunTest(const FString&)
{
    using namespace TerrainForgeToolsetTest;
    FTerrainForgeModule& TerrainForge=FTerrainForgeModule::Get();
    const TSharedPtr<ITerrainGISService> Registered=TerrainForge.GetGISService();
    // With the real service (registered at startup once the GIS module provides one), the shipped profile is listed.
    if(Registered.IsValid())
    {
        const TArray<FTerrainGISProfileSummary> Live=UTerrainForgeToolset::ListGISProfiles();
        const FTerrainGISProfileSummary* Game=Live.FindByPredicate([](const FTerrainGISProfileSummary& P){return P.Id==TEXT("game_level");});
        if(TestNotNull(TEXT("game_level is listed by the registered service"),Game))
        {TestTrue(TEXT("game_level is usable"),Game->Error.IsEmpty());TestFalse(TEXT("game_level has a label"),Game->Label.IsEmpty());TestTrue(TEXT("game_level has requirements"),Game->Requirements.Num()>0);}
    }
    else AddInfo(TEXT("No GIS service is registered at startup; the shipped profile is checked with the service."));

    TerrainForge.UnregisterGISService();
    // No service: an empty list (the raised error reaches the agent through the registry; there is no script frame here).
    TestEqual(TEXT("no service, no profiles"),UTerrainForgeToolset::ListGISProfiles().Num(),0);

    const TSharedRef<FProfilesOnlyService> Fake=MakeShared<FProfilesOnlyService>();
    FTerrainGISProfileInfo Broken;Broken.Id=TEXT("broken");Broken.Path=TEXT("X:/profiles/broken.json");Broken.Error=TEXT("X:/profiles/broken.json: label is required.");
    FTerrainGISProfileInfo Game;Game.Id=TEXT("game_level");Game.Label=TEXT("Game level");Game.Requirements={TEXT("still_water"),TEXT("rivers"),TEXT("glacier")};Game.Path=TEXT("X:/profiles/game_level.json");
    FTerrainGISProfileInfo Own;Own.Id=TEXT("my_level");Own.Label=TEXT("My level");Own.Requirements={TEXT("forest")};Own.Path=TEXT("X:/project/my_level.json");
    Fake->Answer={Broken,Game,Own};
    TerrainForge.RegisterGISService(Fake);
    const TArray<FTerrainGISProfileSummary> Out=UTerrainForgeToolset::ListGISProfiles();
    if(TestEqual(TEXT("three entries"),Out.Num(),3))
    {
        TestEqual(TEXT("usable first, in service order"),Out[0].Id,FString(TEXT("game_level")));
        TestEqual(TEXT("then the project profile"),Out[1].Id,FString(TEXT("my_level")));
        TestEqual(TEXT("errors last"),Out[2].Id,FString(TEXT("broken")));
        TestEqual(TEXT("label"),Out[0].Label,FString(TEXT("Game level")));
        TestEqual(TEXT("requirements in order"),FString::Join(Out[0].Requirements,TEXT(",")),FString(TEXT("still_water,rivers,glacier")));
        TestEqual(TEXT("path"),Out[1].Path,FString(TEXT("X:/project/my_level.json")));
        TestTrue(TEXT("usable has no error"),Out[0].Error.IsEmpty());
        TestEqual(TEXT("error kept verbatim"),Out[2].Error,FString(TEXT("X:/profiles/broken.json: label is required.")));
    }
    TerrainForge.UnregisterGISService();
    if(Registered.IsValid())TerrainForge.RegisterGISService(Registered.ToSharedRef());
    return true;
}
#endif
