#include "MirrorPluginEditor.h"
#include "AssetToolsModule.h"
#include "DoubleBoneMirrorCustomization.h"
#include "MirrorMenuExtender.h"
#include "MirrorPoseDataActions.h"
#include "MirrorPoseDataDetails.h"
#include "Styling/SlateStyleRegistry.h"
#include "SingleBoneMirrorCustomization.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Interfaces/IPluginManager.h"


#define LOCTEXT_NAMESPACE "FMirrorPluginEditorModule"

DEFINE_LOG_CATEGORY(LogMirrorPluginEditor);

void FMirrorPluginEditorModule::StartupModule()
{
	FContentBrowserModule* ContentBrowserModule = FModuleManager::GetModulePtr<FContentBrowserModule>(TEXT("ContentBrowser"));
	if (ContentBrowserModule)
	{
		TArray<FContentBrowserMenuExtender_SelectedAssets>& Extenders = ContentBrowserModule->GetAllAssetViewContextMenuExtenders();

		Extenders.Add(FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&FMirrorMenuExtender::CreateExtender));
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FMirrorMenuExtender::AssetTools = &FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

		PropertyModule.RegisterCustomPropertyTypeLayout("SingleBoneMirror", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSingleBoneMirrorCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout("DoubleBoneMirror", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDoubleBoneMirrorCustomization::MakeInstance));
		PropertyModule.RegisterCustomClassLayout("MirrorPoseData", FOnGetDetailCustomizationInstance::CreateStatic(&FMirrorPoseDataDetails::MakeInstance));
		PropertyModule.NotifyCustomizationModuleChanged();
	}

	{
		Style = MakeShareable(new FSlateStyleSet("MirrorPluginStyle"));

		const TSharedPtr<IPlugin> LoadedPlugin = IPluginManager::Get().FindPlugin("MirrorPlugin");
		const FString PluginContentDirectory = LoadedPlugin->GetContentDir();
		Style->SetContentRoot(PluginContentDirectory);
		const FVector2D MirrorPoseFrontImageSize(128.f, 128.f);

		FSlateImageBrush* MirrorPoseImageBrush = new FSlateImageBrush(
			Style->RootToContentDir(TEXT("Resources/Icon128"), TEXT(".png")),
			MirrorPoseFrontImageSize
		);


		if (MirrorPoseImageBrush)
		{
			Style->Set("ClassThumbnail.MirrorPoseData", MirrorPoseImageBrush);
			FSlateStyleRegistry::RegisterSlateStyle(*Style);
		}

		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		const TSharedPtr<IAssetTypeActions> NewActionTypeOnMenu = MakeShareable(new FMirrorPoseDataActions());
		AssetTools.RegisterAssetTypeActions(NewActionTypeOnMenu.ToSharedRef());
	}
}

void FMirrorPluginEditorModule::ShutdownModule()
{

	FSlateStyleRegistry::UnRegisterSlateStyle(Style->GetStyleSetName());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMirrorPluginEditorModule, MirrorPluginEditor)