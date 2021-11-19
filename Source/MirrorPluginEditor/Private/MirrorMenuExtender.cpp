#include "MirrorMenuExtender.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "Runtime/SlateCore/Public/Widgets/SWidget.h"
#include "AnimGraphNode_MirrorPose.h"
#include "MirrorHelpers.h"
#include "ObjectTools.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetToolsModule.h"
#include "AssetRegistryModule.h"
#include "Toolkits/AssetEditorManager.h"

const FAssetData* FMirrorMenuExtender::SelectedPoseData = nullptr;
FAssetToolsModule* FMirrorMenuExtender::AssetTools = nullptr;

FReply FMirrorMenuExtender::MirrorMenuAction(const FAssetData SelectedAssets)
{
	if (SelectedAssets.GetAsset())
	{
		if (!SelectedPoseData)
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Select an Mirror Pose asset..."));
		}
		else
		{
			UMirrorPoseData* PoseData = Cast<UMirrorPoseData>(SelectedPoseData->GetAsset());
			UAnimSequence* AnimSequence = Cast<UAnimSequence>(SelectedAssets.GetAsset());
			if (PoseData && AnimSequence)
			{
				FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
				const FString Suffix = "_Mirror";
				const FString AnimPath = AnimSequence->GetOutermost()->GetPathName();
				FString Name;
				FString PackageName;
				
				AssetToolsModule.Get().CreateUniqueAssetName(AnimPath, Suffix, PackageName, Name);
				const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
				UObject* NewDuplicatedObject = AssetTools->Get().DuplicateAsset(Name, PackagePath, AnimSequence);
				UAnimSequence* NewAnimSequence = Cast<UAnimSequence>(NewDuplicatedObject);
				MirrorHelpers::ProcessMirrorTrack(NewAnimSequence, PoseData);
			}
		}
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Critic: Select an Anim Sequence asset..."));
	}

	return FReply::Handled();
};

void FMirrorMenuExtender::SelectedAssetOnPicker(const FAssetData& AssetData)
{
	SelectedPoseData = &AssetData;
}

void FMirrorMenuExtender::DeselectedAssetOnPicker()
{
	return;
}

bool FMirrorMenuExtender::OnShouldFilterActor(const AActor* Act)
{
	return false;
}

void FMirrorMenuExtender::CreateSubMenuContent(FMenuBuilder& SubMenuBuilder, const TArray<FAssetData>* SelectedAssets)
{
	SelectedPoseData = nullptr;
	const TSharedRef<SWidget> Selector = PropertyCustomizationHelpers::MakeAssetPickerWithMenu(
		FAssetData(),
		false,
		TArray<const UClass*>{UMirrorPoseData::StaticClass()},
		TArray<UFactory*>{},
		FOnShouldFilterAsset(),
		FOnAssetSelected::CreateStatic(&FMirrorMenuExtender::SelectedAssetOnPicker),
		FSimpleDelegate::CreateStatic(&FMirrorMenuExtender::DeselectedAssetOnPicker));

	const TSharedRef<SWidget> Widget =
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.MaxHeight(300.0f)
		.FillHeight(1.0f)
		[
			SNew(SBox)
			.WidthOverride(220.0f)
		[
			Selector
		]

		]
	+ SVerticalBox::Slot()
		.Padding(0.0f, 8.0f, 0.0f, 0.0f)
		.MaxHeight(20.0f)
		.FillHeight(0.1f)
		[
			SNew(SButton)
			.OnClicked_Static(&FMirrorMenuExtender::MirrorMenuAction, (*SelectedAssets)[0])
		[
			SNew(STextBlock)
			.Text(FText::FromString("Create Mirrored Copy"))
		.Justification(ETextJustify::Center)
		]
		];

	SubMenuBuilder.AddWidget(
		Widget,
		FText::FromString(""));
}

TSharedRef<FExtender> FMirrorMenuExtender::CreateExtender(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> Extender(new FExtender());
	if (SelectedAssets.Num() == 1 && SelectedAssets[0].GetClass() == UAnimSequence::StaticClass())
	{
		FMenuExtensionDelegate ExtensionDelegate;
		ExtensionDelegate.BindLambda([SelectedAssets](FMenuBuilder& Builder)
		{
			Builder.AddSubMenu(
				FText::FromString("Mirror animation"),
				FText::FromString("Mirror animation selection an pose data"),
				FNewMenuDelegate::CreateStatic(&FMirrorMenuExtender::CreateSubMenuContent, &SelectedAssets),
				false,
				FSlateIcon(FEditorStyle::GetStyleSetName(), "PhysicsAssetEditor.Tree.Bone"));

		});

		Extender->AddMenuExtension(
			"GetAssetActions",
			EExtensionHook::After,
			nullptr,
			ExtensionDelegate);
	}
	return Extender;
};