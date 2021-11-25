#pragma once

class FAssetToolsModule;

class MIRRORPLUGINEDITOR_API FMirrorMenuExtender final
{
	
public:
	
	static const FAssetData* SelectedPoseData;

	static FAssetToolsModule* AssetTools;

	static FReply MirrorMenuAction(const FAssetData SelectedAssets);

	static void SelectedAssetOnPicker(const FAssetData& AssetData);

	static void DeselectedAssetOnPicker();

	static bool OnShouldFilterActor(const AActor* Act);

	static void CreateSubMenuContent(FMenuBuilder& SubMenuBuilder, const TArray<FAssetData>* SelectedAssets);

	static TSharedRef<FExtender> CreateExtender(const TArray<FAssetData>& SelectedAssets);
};
