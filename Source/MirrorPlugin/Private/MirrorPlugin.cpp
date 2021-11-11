#include "MirrorPlugin.h"
#include "AssetToolsModule.h"
#include "IAssetTypeActions.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "MirrorPoseDataActions.h"

#define LOCTEXT_NAMESPACE "FMirrorPluginModule"

DEFINE_LOG_CATEGORY(LogMirrorPlugin);

void FMirrorPluginModule::StartupModule()
{
	Style = MakeShareable(new FSlateStyleSet("MirrorPluginStyle"));

	TSharedPtr<IPlugin> LoadedPlugin = IPluginManager::Get().FindPlugin("MirrorPlugin");
	FString PluginContentDirectory = LoadedPlugin->GetContentDir();
	Style->SetContentRoot(PluginContentDirectory);

	FVector2D MirrorPoseFrontImageSize(128.f, 128.f);
	
	FSlateImageBrush* MirrorPoseImageBrush = new FSlateImageBrush(
		Style->RootToContentDir(TEXT("Resources/Icon128"), TEXT(".png")),
		MirrorPoseFrontImageSize
	);

	//if(IsValid()) //n eh uobj;

	if(MirrorPoseImageBrush)
	{
		Style->Set("ClassThumbnail.MirrorPoseData", MirrorPoseImageBrush);
		FSlateStyleRegistry::RegisterSlateStyle(*Style);
	}

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedPtr<IAssetTypeActions> NewActionTypeOnMenu = MakeShareable(new FMirrorPoseDataActions());
	AssetTools.RegisterAssetTypeActions(NewActionTypeOnMenu.ToSharedRef());
	
}

void FMirrorPluginModule::ShutdownModule()
{

	FSlateStyleRegistry::UnRegisterSlateStyle(Style->GetStyleSetName());
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMirrorPluginModule, MirrorPlugin)