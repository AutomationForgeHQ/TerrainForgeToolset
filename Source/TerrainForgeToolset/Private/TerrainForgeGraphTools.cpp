#include "TerrainForgeToolset.h"
#include "TerrainForgeGraphSubsystem.h"
#include "TerrainForgeSubsystem.h"
#include "Editor.h"
#include "Interfaces/IPluginManager.h"
#include "UObject/StrongObjectPtr.h"
namespace
{
UTerrainForgeGraphSubsystem* Core(){auto* S=GEditor?GEditor->GetEditorSubsystem<UTerrainForgeGraphSubsystem>():nullptr;if(!S)UTerrainForgeSubsystem::Fail(TEXT("TerrainForge requires an editor session. Enable its core plugin and retry."));return S;}
UTerrainDef* Def(const FString& Path){auto* D=LoadObject<UTerrainDef>(nullptr,*Path);if(!D)UTerrainForgeSubsystem::Fail(TEXT("Definition not found. Call List Definitions or Create Definition."));return D;}
FTerrainGraphSnapshot Snapshot(UTerrainDef* D){FTerrainGraphSnapshot S;if(D){S.DefinitionPath=D->GetPathName();S.Revision=UTerrainForgeGraphSubsystem::Revision(D);S.GraphEnabled=D->bUseGraph;S.Graph=D->Graph;}return S;}
FTerrainRasterInfo Info(const FTerrainRasterSource& S){FTerrainRasterInfo I;I.Id=S.Id;I.Name=S.Name;I.Format=S.Format;I.RawSize=S.RawSize;I.ByteCount=S.Bytes.Num();return I;}
/** Async lifetime is transport-only. All real work belongs to the core subsystem. */
template<class TResult,class TValue>
TResult* Async(UTerrainDef* D,double Deadline,TFunction<void(TFunction<void(const TValue&,const FString&)>)> Start)
{
    auto* R=NewObject<TResult>();R->AddToRoot();auto Finished=MakeShared<bool>(false);auto Keep=MakeShared<TStrongObjectPtr<UTerrainDef>>(D);double Began=FPlatformTime::Seconds();
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([R,Finished,Keep,Began,Deadline](float){if(*Finished)return false;if(!GEditor||FPlatformTime::Seconds()-Began>Deadline){*Finished=true;R->SetError(TEXT("Operation wait expired. Inspect terrain/GIS status before retrying; an accepted job may still finish."));R->RemoveFromRoot();return false;}return true;}),.25f);
    Start([R,Finished,Keep](const TValue& V,const FString& E){if(*Finished)return;*Finished=true;if(E.IsEmpty())R->SetValue(V);else R->SetError(E);R->RemoveFromRoot();});return R;
}
}
FString UTerrainForgeToolset::GetToolsetVersion() const{auto P=IPluginManager::Get().FindPlugin(TEXT(UE_PLUGIN_NAME));return P?P->GetDescriptor().VersionName:TEXT("unavailable");}
FTerrainGraphSnapshot UTerrainForgeToolset::GetGraph(const FString& P){return Snapshot(Def(P));}
TArray<FTerrainNodeSpec> UTerrainForgeToolset::ListNodeTypes(const FString& P){auto* C=Core();return C?C->GetCatalog(Def(P)):TArray<FTerrainNodeSpec>();}
TArray<FString> UTerrainForgeToolset::ValidateGraph(const FString& P,const FTerrainGraphData& G){auto* C=Core();return C?C->ValidateGraph(Def(P),G):TArray<FString>{TEXT("Core unavailable.")};}
bool UTerrainForgeToolset::ReplaceGraph(const FString& P,const FString& R,const FTerrainGraphData& G){auto* C=Core();return C&&C->ReplaceGraph(Def(P),R,G);}
FTerrainGraphNodeData UTerrainForgeToolset::AddNode(const FString& P,const FString& R,const FString& T,FVector2D XY){auto* C=Core();return C?C->AddNode(Def(P),R,T,XY):FTerrainGraphNodeData();}
bool UTerrainForgeToolset::SetNode(const FString& P,const FString& R,const FTerrainGraphNodeData& N){auto* C=Core();return C&&C->SetNode(Def(P),R,N);}
bool UTerrainForgeToolset::RemoveNode(const FString& P,const FString& R,const FString& N){auto* C=Core();return C&&C->RemoveNode(Def(P),R,N);}
bool UTerrainForgeToolset::ConnectNodes(const FString& P,const FString& R,const FString& Target,const FString& Input,const FTerrainGraphLink& L){auto* C=Core();return C&&C->Connect(Def(P),R,Target,Input,L);}
bool UTerrainForgeToolset::DisconnectNodes(const FString& P,const FString& R,const FString& Target,const FString& Input){auto* C=Core();return C&&C->Disconnect(Def(P),R,Target,Input);}
bool UTerrainForgeToolset::SetGraphOutput(const FString& P,const FString& R,const FString& N){auto* C=Core();return C&&C->SetOutput(Def(P),R,N);}
UToolCallAsyncResultTerrainCatalog* UTerrainForgeToolset::RefreshNodeCatalog(const FString& P){auto* D=Def(P);return Async<UToolCallAsyncResultTerrainCatalog,TArray<FTerrainNodeSpec>>(D,200,[D](auto Done){if(auto* C=Core();C&&D)C->RefreshCatalog(D,MoveTemp(Done));else Done({},TEXT("Select an existing Terrain Definition."));});}
UToolCallAsyncResultTerrainPreview* UTerrainForgeToolset::PreviewNode(const FString& P,const FString& N,const FString& O,int32 Resolution){auto* D=Def(P);return Async<UToolCallAsyncResultTerrainPreview,FTerrainPreviewInfo>(D,400,[D,N,O,Resolution](auto Done){if(auto* C=Core();C&&D)C->Preview(D,N,O,Resolution,MoveTemp(Done));else Done({},TEXT("Select an existing Terrain Definition."));});}
TArray<FTerrainRasterInfo> UTerrainForgeToolset::ListSources(const FString& P){TArray<FTerrainRasterInfo> R;if(auto* D=Def(P))for(const auto& S:D->Sources)R.Add(Info(S));return R;}
UToolCallAsyncResultTerrainRaster* UTerrainForgeToolset::ImportRaster(const FString& P,const FString& F,int32 RawSize){auto* D=Def(P);return Async<UToolCallAsyncResultTerrainRaster,FTerrainRasterInfo>(D,200,[D,F,RawSize](auto Done){if(auto* C=Core();C&&D)C->ImportRaster(D,F,RawSize,[Done=MoveTemp(Done)](const auto& S,const FString& E){Done(Info(S),E);});else Done({},TEXT("Select an existing Terrain Definition."));});}
UToolCallAsyncResultTerrainProject* UTerrainForgeToolset::ImportProject(const FString& P,const FString& F){auto* D=Def(P);return Async<UToolCallAsyncResultTerrainProject,FTerrainGraphSnapshot>(D,400,[D,F](auto Done){if(auto* C=Core();C&&D)C->ImportProjectFile(D,F,[D,Done=MoveTemp(Done)](const FString& E){Done(Snapshot(D),E);});else Done({},TEXT("Select an existing Terrain Definition."));});}
bool UTerrainForgeToolset::SaveProject(const FString& P,const FString& F){auto* C=Core();if(!C)return false;const bool OK=C->SaveProjectFile(Def(P),F);if(!OK)UTerrainForgeSubsystem::Fail(TEXT("Project could not be saved. Select a graph definition and a writable JSON filename, then retry."));return OK;}
UToolCallAsyncResultTerrainGISDiscovery* UTerrainForgeToolset::DiscoverGIS(const FTerrainGISSelection& S){return Async<UToolCallAsyncResultTerrainGISDiscovery,TArray<FTerrainGISDataset>>(nullptr,200,[S](auto Done){if(auto* C=Core())C->DiscoverGIS(S,MoveTemp(Done));else Done({},TEXT("Enable TerrainForge core."));});}
UToolCallAsyncResultTerrainGIS* UTerrainForgeToolset::ImportGIS(const FString& P,const FTerrainGISSelection& S,bool Replace){auto* D=Def(P);return Async<UToolCallAsyncResultTerrainGIS,FTerrainGISStatus>(D,1500,[D,S,Replace](auto Done){if(auto* C=Core();C&&D)C->ImportGIS(D,S,Replace,MoveTemp(Done));else Done({},TEXT("Select an existing Terrain Definition."));});}
UToolCallAsyncResultTerrainGISAssessment* UTerrainForgeToolset::AssessGIS(const FTerrainGISSelection& S,const FString& Profile){return Async<UToolCallAsyncResultTerrainGISAssessment,FTerrainGISAssessment>(nullptr,200,[S,Profile](auto Done){if(auto* C=Core())C->AssessGIS(S,Profile,MoveTemp(Done));else Done({},TEXT("Enable TerrainForge core."));});}
UToolCallAsyncResultTerrainGIS* UTerrainForgeToolset::CreateLevelProject(const FString& P,const FTerrainGISSelection& S,const FString& Profile){auto* D=Def(P);return Async<UToolCallAsyncResultTerrainGIS,FTerrainGISStatus>(D,1500,[D,S,Profile](auto Done){if(auto* C=Core();C&&D)C->ImportLevelProject(D,S,Profile,MoveTemp(Done));else Done({},TEXT("Select an existing Terrain Definition."));});}
TArray<FTerrainGISProfileSummary> UTerrainForgeToolset::ListGISProfiles()
{
    // Through the GIS service, like the other GIS tools: the service owns the profile folders and their order.
    const TSharedPtr<ITerrainGISService> Service=UTerrainForgeGraphSubsystem::GISService();
    if(!Service){UTerrainForgeSubsystem::Fail(FString(UTerrainForgeGraphSubsystem::GISNotLoadedError())+TEXT(" Enable the TerrainForge plugin and restart the editor, then call List GIS Profiles again."));return {};}
    TArray<FTerrainGISProfileSummary> Out;
    for(const FTerrainGISProfileInfo& P:Service->Profiles())
    {FTerrainGISProfileSummary S;S.Id=P.Id;S.Label=P.Label;S.Requirements=P.Requirements;S.Path=P.Path;S.Error=P.Error;Out.Add(MoveTemp(S));}
    // Usable profiles first, then the files that could not be read; the service already orders them so, the sort keeps it stable.
    Out.StableSort([](const FTerrainGISProfileSummary& A,const FTerrainGISProfileSummary& B){return A.Error.IsEmpty()&&!B.Error.IsEmpty();});
    return Out;
}
FTerrainGISStatus UTerrainForgeToolset::GetGISStatus(const FString& P){auto* D=Def(P);return D?D->GISStatus:FTerrainGISStatus();}
void UTerrainForgeToolset::CancelGIS(const FString& P){if(auto* C=Core())C->CancelGIS(Def(P));}
TArray<FString> UTerrainForgeToolset::ListArtifacts(const FString& P){auto* C=Core();return C?C->ListArtifacts(Def(P)):TArray<FString>();}
FTerrainExportSettings UTerrainForgeToolset::GetExportSettings(const FString& P){auto* D=Def(P);return D?D->ExportSettings:FTerrainExportSettings();}
bool UTerrainForgeToolset::SetExportSettings(const FString& P,const FString& R,const FTerrainExportSettings& S){auto* C=Core();return C&&C->SetExportSettings(Def(P),R,S);}
TArray<FString> UTerrainForgeToolset::ValidateExport(const FString& P){auto* C=Core();return C?C->ValidateExport(Def(P)):TArray<FString>{TEXT("Core unavailable.")};}
// The core raises the reason once (refusal or write failure) and returns it in Warnings; nothing is added here.
FTerrainExportResult UTerrainForgeToolset::ExportAssets(const FString& P){auto* C=Core();if(!C)return {};return C->ExportAssets(Def(P));}
