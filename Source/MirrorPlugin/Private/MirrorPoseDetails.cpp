#if WITH_EDITOR
#include "MirrorPoseDetails.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Editor/PropertyEditor/Public/IDetailChildrenBuilder.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "Editor/PropertyEditor/Public/IDetailPropertyRow.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"
#include "Animation/AnimInstance.h"
#include "Runtime/SlateCore/Public/Widgets/SBoxPanel.h"
#include "PropertyCustomizationHelpers.h"
#include "Runtime/UMG/Public/Components/ComboBoxString.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "Editor/UnrealEd/Public/ObjectTools.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Runtime/Core/Public/Misc/MessageDialog.h"
#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "MirrorPoseData.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SSpinBox.h"



namespace AnimSequence_MirrorMenu
{
	const FAssetData* SelectedPoseData;

	FAssetToolsModule* AssetTools;

	static FReply MirrorMenuAction(FAssetData SelectedAssets)
	{
		if (SelectedAssets.GetAsset())
		{
			if (!SelectedPoseData)
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Select an Mirror Pose asset..."));
			}
			else
			{
				UMirrorPoseData* PoseData = Cast<UMirrorPoseData>(AnimSequence_MirrorMenu::SelectedPoseData->GetAsset());
				UAnimSequence* AnimSequence = Cast<UAnimSequence>(SelectedAssets.GetAsset());
				if (PoseData && AnimSequence)
				{
					TSet<UPackage*> ReturnedPackage;
					ObjectTools::FPackageGroupName PackageSettings;
					PackageSettings.ObjectName = AnimSequence->GetName() + "_Mirror";
					PackageSettings.PackageName = AnimSequence->GetOutermost()->GetPathName();

					UObject* NewDuplicatedObject = AssetTools->Get().DuplicateAsset((SelectedAssets.AssetName).ToString() + "_Mirror", SelectedAssets.PackagePath.ToString(), AnimSequence);
					
					UAnimSequence* NewAnimSequence = Cast<UAnimSequence>(NewDuplicatedObject);
					UMirrorPoseData::ProcessMirrorTrack(PoseData, NewAnimSequence);
				}	
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Critic: Select an Anim Sequence asset..."));
		}
		
		return FReply::Handled();
	};

	static void SelectedAssetOnPicker(const FAssetData& AssetData)
	{
		SelectedPoseData = &AssetData;
	}

	static void DeselectedAssetOnPicker() {}

	static bool OnShouldFilterActor(const AActor* Act)
	{
		return false;
	}

	static void CreateSubMenuContent(FMenuBuilder& SubMenuBuilder, const TArray<FAssetData>* SelectedAssets)
	{
		SelectedPoseData = nullptr;
		TSharedRef<SWidget> Selector = PropertyCustomizationHelpers::MakeAssetPickerWithMenu(
		FAssetData(),
		false,
		TArray<const UClass*>{UMirrorPoseData::StaticClass()},
		TArray<UFactory*>{},
		FOnShouldFilterAsset(),
		FOnAssetSelected::CreateStatic(&AnimSequence_MirrorMenu::SelectedAssetOnPicker),
		FSimpleDelegate::CreateStatic(&AnimSequence_MirrorMenu::DeselectedAssetOnPicker));

		TSharedRef<SWidget> Widget =
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
			.OnClicked_Static(&AnimSequence_MirrorMenu::MirrorMenuAction, (*SelectedAssets)[0])
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

	static TSharedRef<FExtender> CreateExtender(const TArray<FAssetData>& SelectedAssets)
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
					FNewMenuDelegate::CreateStatic(&AnimSequence_MirrorMenu::CreateSubMenuContent, &SelectedAssets),
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
};


struct FU_Details_Inicialization
{
	FU_Details_Inicialization()
	{
		/* CASO SEJA IMPLEMENTADO EM OUTRO PROJETO, LEMBRAR DE REGISTRAR A CUSTOMIZACAO, COMO ABAIXO, EM ALGUMA CLASSE CONSTRUTORA QUE NAO SE REPITA,
		OU DEIXAR AQUI MESMO, USANDO UMA INSTANCIA EXECUTADA DE UMA STRUCT STATICA*/

		FContentBrowserModule* ContentBrowserModule = FModuleManager::GetModulePtr<FContentBrowserModule>(TEXT("ContentBrowser"));
		if (ContentBrowserModule)
		{
			TArray<FContentBrowserMenuExtender_SelectedAssets>& Extenders = ContentBrowserModule->GetAllAssetViewContextMenuExtenders();

			Extenders.Add(FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&AnimSequence_MirrorMenu::CreateExtender));
			FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

			AnimSequence_MirrorMenu::AssetTools = &FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

			PropertyModule.RegisterCustomPropertyTypeLayout("SingleBoneMirror", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSingleBoneMirrorCustomization::MakeInstance));
			PropertyModule.RegisterCustomPropertyTypeLayout("DoubleBoneMirror", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDoubleBoneMirrorCustomization::MakeInstance));
			PropertyModule.RegisterCustomClassLayout("MirrorPoseData", FOnGetDetailCustomizationInstance::CreateStatic(&FMirrorPoseDataDetails::MakeInstance));
			PropertyModule.NotifyCustomizationModuleChanged();
		}
	}
};

static FU_Details_Inicialization ConstructorInstance;


/* --------------------------------------------------------------------------------------------- */




/* --------------------------------------------------------------------------------------------- */

TSharedRef<IDetailCustomization> FMirrorPoseDataDetails::MakeInstance()
{
	return MakeShareable(new FMirrorPoseDataDetails);
}

TSharedRef<SWidget> FMirrorPoseDataDetails::MakeOption(TSharedPtr<FString> Option)
{
	return SNew(STextBlock).Text(FText::FromString(*Option));
}

void FMirrorPoseDataDetails::OnSelection(TSharedPtr<FString> Option, ESelectInfo::Type)
{
	if (!Option.IsValid()) return;
	TArray<TWeakObjectPtr<UObject>> OutObjects;
	Builder->GetObjectsBeingCustomized(OutObjects);
	UMirrorPoseData* Object = Cast<UMirrorPoseData>(OutObjects[0]);
	Object->RootBoneName = *Option;
	CurrentSelectItem = Option;
	/* ------------------------------------------------- */
	// Create Package	
	/*UAnimSequence* AnimSequenceObject = Object->xx; 

	if (!AnimSequenceObject) return;
	TSet<UPackage*> ReturnedPackage;
	ObjectTools::FPackageGroupName PackageSettings;
	PackageSettings.ObjectName = AnimSequenceObject->GetName() + "_Mirror";
	PackageSettings.PackageName = AnimSequenceObject->GetOutermost()->GetPathName();
	
	UObject* NewDuplicatedObject = ObjectTools::DuplicateSingleObject(AnimSequenceObject, PackageSettings, ReturnedPackage);
	UAnimSequence* NewAnimSequence = Cast<UAnimSequence>(NewDuplicatedObject);
	
	NewAnimSequence->GetRawAnimationTrack(0).PosKeys[0].Set(500.0f, 0.0f, 200.0f);
	NewAnimSequence->GetRawAnimationTrack(0).PosKeys[1].Set(500.0f, 0.0f, 200.0f);
	NewAnimSequence->GetRawAnimationTrack(0).PosKeys[2].Set(500.0f, 0.0f, 200.0f);
	NewAnimSequence->GetRawAnimationTrack(0).PosKeys[3].Set(500.0f, 0.0f, 200.0f);
	NewAnimSequence->GetRawAnimationTrack(0).PosKeys[4].Set(500.0f, 0.0f, 200.0f);
	NewAnimSequence->GetRawAnimationTrack(0).PosKeys[5].Set(500.0f, 0.0f, 200.0f);

	NewAnimSequence->CompressRawAnimData();*/
	
	/* ------------------------------------------------- */
	Builder->ForceRefreshDetails();
}

FText FMirrorPoseDataDetails::GetCurrentItem() const
{
	if (CurrentSelectItem.IsValid())
	{
		return FText::FromString(*CurrentSelectItem);
	}
	else
	{
		return FText::FromString("None");
	}
}

void FMirrorPoseDataDetails::GenerateViewportPlane(IDetailLayoutBuilder& DetailBuilder, USkeleton* SelectedSkeleton)
{
	IDetailCategoryBuilder& VisualEditorMirrorSelector = DetailBuilder.EditCategory("Mirror Plane Setup");
	const FText FilterString = FText::FromString(TEXT("Visual Editor Mirror Selector"));
	FDetailWidgetRow& VisualEditorMirrorRow = VisualEditorMirrorSelector.AddCustomRow(FilterString);

	const float PlaneViewportWidth = 550.0f;
	const float WholeViewportSize = 700.0f;
	VisualEditorMirrorRow.WholeRowContent()
	[
		SNew(SBox)
		.HAlign(EHorizontalAlignment::HAlign_Center)
		//.MinDesiredWidth(WholeViewportSize)
		//.Padding(FMargin(-WholeViewportSize/2.0f, 0.0f, 0.0f, 0.0f))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		        .AutoWidth()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(10.0f)
				.AutoHeight()
				[
					SNew(SBox).WidthOverride(PlaneViewportWidth)
					[
						SAssignNew(PlaneViewport, SPlaneViewport).Skeleton(Cast<USkeleton>(SelectedSkeleton))
					]
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(10.0f)
			.AutoWidth()
			[
			    GenerateViewportPlaneControls()
			]
		]
		
	];
}


const TSharedRef<SBox> FMirrorPoseDataDetails::GenerateViewportPlaneControls()
{
	const FOnFloatValueChanged OnValueChangedHandle = FOnFloatValueChanged::CreateRaw(this, &FMirrorPoseDataDetails::ViewportPlaneControl_SliderAction);
	return SNew(SBox).WidthOverride(100.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		        .AutoWidth()
		        .Padding(5.0f)
		        [
			  SNew(SVerticalBox)
			  + SVerticalBox::Slot()
		          .AutoHeight()
		          .Padding(FMargin(0.0f, 0.0f, 0.0f, 8.0f))
		          .VAlign(EVerticalAlignment::VAlign_Bottom)
		          [
			    SNew(STextBlock).Text(FText::FromString("Mirror Plane Angle"))
		          ]
	                  + SVerticalBox::Slot()
		          .AutoHeight()
			  .Padding(FMargin(0.0f, 0.0f, 0.0f, 8.0f))
			  .VAlign(EVerticalAlignment::VAlign_Bottom)
		          [
			    SNew(STextBlock).Text(FText::FromString("Central Bones Acceptance"))
		          ]
			]
	                + SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f)
		        [
			  SNew(SVerticalBox)
			  + SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(FMargin(0.0f, 0.0f, 0.0f, 5.0f))
			  .VAlign(EVerticalAlignment::VAlign_Center)
			  [
				  SNew(SSpinBox<float>)
				  .MaxValue(360.0f)
				  .MinValue(0.0f)
				  .MinDesiredWidth(100.0f)
				  .OnValueChanged(FOnFloatValueChanged::CreateRaw(this, &FMirrorPoseDataDetails::ViewportPlaneControl_SliderAction))
			  ]
			  + SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(FMargin(0.0f, 0.0f, 0.0f, 5.0f))
			  .VAlign(EVerticalAlignment::VAlign_Center)
			  [
				  SNew(SSpinBox<float>).MaxValue(50.0f).MinValue(0.0f).MinDesiredWidth(100.0f)
			  ]
		        ]
		];
}

void FMirrorPoseDataDetails::ViewportPlaneControl_SliderAction(float NewValue)
{
	if(PlaneViewport.IsValid())
	{
		PlaneViewport->GetVisualMirrorPlane()->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f) + FRotator(0.0f, NewValue, 0.0f));
	}
	
}

void FMirrorPoseDataDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	//Builder = &DetailBuilder;
	UObject* SelectedSkeleton;
	DetailBuilder.GetProperty("Skeleton")->GetValue(SelectedSkeleton);
	DetailBuilder.GetProperty("Skeleton")->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([&]() { DetailBuilder.ForceRefreshDetails(); }));

	if (SelectedSkeleton)
	{
		GenerateViewportPlane(DetailBuilder, Cast<USkeleton>(SelectedSkeleton));
		
		USkeleton* Skeleton = Cast<USkeleton>(SelectedSkeleton);
		IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory("GeneralSettings");
		FDetailWidgetRow& RootBoneNameRow = CategoryBuilder.AddProperty("RootBoneName").CustomWidget();
		//CategoryBuilder.AddProperty("bMirrorRootBoneTranslation");
		CategoryBuilder.AddProperty("SingleBones");
		CategoryBuilder.AddProperty("DoubleBones");
	
		FString CurrentRootBoneName;
		DetailBuilder.GetProperty("RootBoneName")->GetValue(CurrentRootBoneName);
		for (int BoneIndex = 0; BoneIndex < Skeleton->GetReferenceSkeleton().GetNum(); BoneIndex++)
		{
			TSharedPtr<FString> Item = MakeShareable(new FString(Skeleton->GetReferenceSkeleton().GetBoneName(BoneIndex).ToString()));

			if (Skeleton->GetReferenceSkeleton().GetBoneName(BoneIndex).ToString() == CurrentRootBoneName)
			{
				CurrentSelectItem = Item;
			}

			Options.Add(Item);
		}

		RootBoneNameRow.NameContent()
		[
			SNew(STextBlock).Text(FText::FromString("Root Bone Name"))
		];
		RootBoneNameRow.ValueContent()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&Options)
			.OnGenerateWidget(this, &FMirrorPoseDataDetails::MakeOption)
			.OnSelectionChanged(this, &FMirrorPoseDataDetails::OnSelection)
			.InitiallySelectedItem(CurrentSelectItem)
			[
				SNew(STextBlock)
				.Text(this, &FMirrorPoseDataDetails::GetCurrentItem)
			]
		];
	}
	else
	{
		DetailBuilder.HideCategory("GeneralSettings");
	}
}

/* --------------------------------------------------------------------------------------------- */

TSharedRef<IPropertyTypeCustomization> FSingleBoneMirrorCustomization::MakeInstance()
{
	return MakeShareable(new FSingleBoneMirrorCustomization());
}

TSharedRef<SWidget> FSingleBoneMirrorCustomization::BoolWidget(TSharedRef<SWidget> BoolProperty, FString Text)
{
	TSharedRef<SWidget> GeneratedWidget = 
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(FMargin(4.0f, 1.5f, 4.0f, 0.0f))
	[
		SNew(STextBlock)
		.Text(FText::FromString(Text))
	]
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		BoolProperty
	];

	return GeneratedWidget;
}

TSharedRef<SWidget> FSingleBoneMirrorCustomization::MakeOption(TSharedPtr<FString> Option)
{
	return SNew(STextBlock).Text(FText::FromString(*Option));
}

void FSingleBoneMirrorCustomization::OnSelection(TSharedPtr<FString> Option, ESelectInfo::Type)
{
	if (!Option.IsValid()) return;
	StructHandle->GetChildHandle("BoneName")->SetValue(*Option);
	CurrentSelectItem = Option;
	Builder->GetParentCategory().GetParentLayout().ForceRefreshDetails();
}

FText FSingleBoneMirrorCustomization::GetCurrentItem() const
{
	if (CurrentSelectItem.IsValid())
	{
		return FText::FromString(*CurrentSelectItem);
	}
	else
	{
		return FText::FromString("None");
	}
}

bool FSingleBoneMirrorCustomization::CheckSkeletonSelected(UMirrorPoseData*& OuterInstance, USkeleton*& Skeleton)
{
	TArray<UObject*> OuterObjects;
	StructHandle->GetOuterObjects(OuterObjects);
	OuterInstance = Cast<UMirrorPoseData>(OuterObjects[0]);

	if (OuterInstance)
	{
		if (OuterInstance->Skeleton) Skeleton = OuterInstance->Skeleton; return true;
	}

	return false;
}

void FSingleBoneMirrorCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;

	UMirrorPoseData* OuterInstance;
	USkeleton* TargetSkeleton;
	if (!CheckSkeletonSelected(OuterInstance, TargetSkeleton)) return;

	FString CurrentSelectBone;
	StructPropertyHandle->GetChildHandle("BoneName")->GetValue(CurrentSelectBone);

	FString ValueContentName = (!CurrentSelectBone.IsEmpty() && TargetSkeleton->GetReferenceSkeleton().FindBoneIndex(*CurrentSelectBone) != INDEX_NONE) ? CurrentSelectBone : "None";

	HeaderRow.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Bone Settings"))
	];	

	HeaderRow.ValueContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("["+ ValueContentName +"]"))
		.ColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.0f, 0.0f)))
		.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
	];
}

void FSingleBoneMirrorCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	Builder = &StructBuilder;

	UMirrorPoseData* OuterInstance;
	USkeleton* TargetSkeleton;

	if (!CheckSkeletonSelected(OuterInstance, TargetSkeleton)) return;

	/*----------------------------------- Bone Selector Content -------------------------------*/
	FDetailWidgetRow& BoneSelectorRow = StructBuilder.AddCustomRow(FText::FromString("Bone Selector"));

	const FReferenceSkeleton& RefSkeleton = TargetSkeleton->GetReferenceSkeleton();
	FString String_CurrentSelectBone;
	StructPropertyHandle->GetChildHandle("BoneName")->GetValue(String_CurrentSelectBone);

	for (int BoneIndex = 0; BoneIndex < RefSkeleton.GetNum(); BoneIndex++)
	{
		TSharedPtr<FString> Item = MakeShareable(new FString(RefSkeleton.GetBoneName(BoneIndex).ToString()));

		if (RefSkeleton.GetBoneName(BoneIndex).ToString() == String_CurrentSelectBone)
		{
			CurrentSelectItem = Item;
		}

		Options.Add(Item);
	}
		
	BoneSelectorRow.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Bone"))
	];

	BoneSelectorRow.ValueContent()
	[
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&Options)
		.OnGenerateWidget(this, &FSingleBoneMirrorCustomization::MakeOption)
		.OnSelectionChanged(this, &FSingleBoneMirrorCustomization::OnSelection)
		.InitiallySelectedItem(CurrentSelectItem)
		[
			SNew(STextBlock)
			.Text(this, &FSingleBoneMirrorCustomization::GetCurrentItem)
		]
	];



	/*----------------------------------- Rotation Axis Content -------------------------------*/

	FDetailWidgetRow& RotationAxisRow = StructBuilder.AddCustomRow(FText::FromString("Rotation Axis"));

	RotationAxisRow.ValueContent()
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("RotationX")->CreatePropertyValueWidget(), "X")
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("RotationY")->CreatePropertyValueWidget(), "Y")
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("RotationZ")->CreatePropertyValueWidget(), "Z")
		]
	];

	RotationAxisRow.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Rotation axis"))
	];

	/*---------------------------------- Location Axis Content -----------------------------------*/

	FDetailWidgetRow& LocationAxisRow = StructBuilder.AddCustomRow(FText::FromString("Location Axis"));

	LocationAxisRow.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("LocationX")->CreatePropertyValueWidget(), "X")
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("LocationY")->CreatePropertyValueWidget(), "Y")
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("LocationZ")->CreatePropertyValueWidget(), "Z")
		]
	];

	LocationAxisRow.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Location axis"))
	];
}

/* --------------------------------------------------------------------------------------------- */

TSharedRef<IPropertyTypeCustomization> FDoubleBoneMirrorCustomization::MakeInstance()
{
	return MakeShareable(new FDoubleBoneMirrorCustomization());
}

TSharedRef<SWidget> FDoubleBoneMirrorCustomization::BoolWidget(TSharedRef<SWidget> BoolProperty, FString Text)
{
	TSharedRef<SWidget> GeneratedWidget =
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(4.0f, 1.5f, 4.0f, 0.0f))
		[
			SNew(STextBlock)
			.Text(FText::FromString(Text))
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolProperty
		];

	return GeneratedWidget;
}

TSharedRef<SWidget> FDoubleBoneMirrorCustomization::MakeOption(TSharedPtr<FString> Option)
{
	return SNew(STextBlock).Text(FText::FromString(*Option));
}

void FDoubleBoneMirrorCustomization::LOnSelection(TSharedPtr<FString> Option, ESelectInfo::Type)
{
	if (!Option.IsValid()) return;
	StructHandle->GetChildHandle("LBoneName")->SetValue(*Option);
	LCurrentSelectItem = Option;
	Builder->GetParentCategory().GetParentLayout().ForceRefreshDetails();
}

void FDoubleBoneMirrorCustomization::ROnSelection(TSharedPtr<FString> Option, ESelectInfo::Type)
{
	if (!Option.IsValid()) return;
	StructHandle->GetChildHandle("RBoneName")->SetValue(*Option);
	RCurrentSelectItem = Option;
	Builder->GetParentCategory().GetParentLayout().ForceRefreshDetails();
}

FText FDoubleBoneMirrorCustomization::RGetCurrentItem() const
{
	if (RCurrentSelectItem.IsValid())
	{
		return FText::FromString(*RCurrentSelectItem);
	}
	else
	{
		return FText::FromString("None");
	}
}

FText FDoubleBoneMirrorCustomization::LGetCurrentItem() const
{
	if (LCurrentSelectItem.IsValid())
	{
		return FText::FromString(*LCurrentSelectItem);
	}
	else
	{
		return FText::FromString("None");
	}
}

bool FDoubleBoneMirrorCustomization::CheckSkeletonSelected(UMirrorPoseData*& OuterInstance, USkeleton*& Skeleton)
{
	TArray<UObject*> OuterObjects;
	StructHandle->GetOuterObjects(OuterObjects);
	OuterInstance = Cast<UMirrorPoseData>(OuterObjects[0]);

	if (OuterInstance)
	{
		if (OuterInstance->Skeleton) Skeleton = OuterInstance->Skeleton; return true;
	}

	return false;
}

void FDoubleBoneMirrorCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;

	UMirrorPoseData* OuterInstance;
	USkeleton* TargetSkeleton;
	if (!CheckSkeletonSelected(OuterInstance, TargetSkeleton)) return;

	FString LCurrentSelectBone;
	StructPropertyHandle->GetChildHandle("LBoneName")->GetValue(LCurrentSelectBone);

	FString RCurrentSelectBone;
	StructPropertyHandle->GetChildHandle("RBoneName")->GetValue(RCurrentSelectBone);

	FString LValueContentName = (!LCurrentSelectBone.IsEmpty() && TargetSkeleton->GetReferenceSkeleton().FindBoneIndex(*LCurrentSelectBone) != INDEX_NONE) ? LCurrentSelectBone : "None";
	FString RValueContentName = (!RCurrentSelectBone.IsEmpty() && TargetSkeleton->GetReferenceSkeleton().FindBoneIndex(*RCurrentSelectBone) != INDEX_NONE) ? RCurrentSelectBone : "None";

	HeaderRow.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Bone Settings"))
	];

	HeaderRow.ValueContent()
	.MinDesiredWidth(250.0f)
	[
		SNew(STextBlock)
		.Text(FText::FromString("[" + LValueContentName + "]" + " <---> " + "[" + RValueContentName + "]"))
		.ColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.0f, 0.0f)))
		.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
	];
}

void FDoubleBoneMirrorCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	Builder = &StructBuilder;

	UMirrorPoseData* OuterInstance;
	USkeleton* TargetSkeleton;

	if (!CheckSkeletonSelected(OuterInstance, TargetSkeleton)) return;

	/*----------------------------------- Bone Selector Content -------------------------------*/
	FDetailWidgetRow& BoneSelectorRow = StructBuilder.AddCustomRow(FText::FromString("Bone Selector"));

	const FReferenceSkeleton& RefSkeleton = TargetSkeleton->GetReferenceSkeleton();
	
	FString LCurrentSelectBone;
	StructPropertyHandle->GetChildHandle("LBoneName")->GetValue(LCurrentSelectBone);

	FString RCurrentSelectBone;
	StructPropertyHandle->GetChildHandle("RBoneName")->GetValue(RCurrentSelectBone);

	for (int BoneIndex = 0; BoneIndex < RefSkeleton.GetNum(); BoneIndex++)
	{
		TSharedPtr<FString> Item = MakeShareable(new FString(RefSkeleton.GetBoneName(BoneIndex).ToString()));

		if (RefSkeleton.GetBoneName(BoneIndex).ToString() == LCurrentSelectBone)
		{
			LCurrentSelectItem = Item;
		}

		if (RefSkeleton.GetBoneName(BoneIndex).ToString() == RCurrentSelectBone)
		{
			RCurrentSelectItem = Item;
		}

		Options.Add(Item);
	}

	BoneSelectorRow.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Bones"))
	];

	BoneSelectorRow.ValueContent()
	.MinDesiredWidth(250.0f)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&Options)
			.OnGenerateWidget(this, &FDoubleBoneMirrorCustomization::MakeOption)
			.OnSelectionChanged(this, &FDoubleBoneMirrorCustomization::LOnSelection)
			.InitiallySelectedItem(LCurrentSelectItem)
			[
				SNew(STextBlock)
				.Text(this, &FDoubleBoneMirrorCustomization::LGetCurrentItem)
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(" <---> "))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&Options)
			.OnGenerateWidget(this, &FDoubleBoneMirrorCustomization::MakeOption)
			.OnSelectionChanged(this, &FDoubleBoneMirrorCustomization::ROnSelection)
			.InitiallySelectedItem(RCurrentSelectItem)
			[
				SNew(STextBlock)
				.Text(this, &FDoubleBoneMirrorCustomization::RGetCurrentItem)
			]
		]
	];



	/*----------------------------------- Rotation Axis Content -------------------------------*/

	FDetailWidgetRow& RotationAxisRow = StructBuilder.AddCustomRow(FText::FromString("Rotation Axis"));

	RotationAxisRow.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("RotationX")->CreatePropertyValueWidget(), "X")
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("RotationY")->CreatePropertyValueWidget(), "Y")
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("RotationZ")->CreatePropertyValueWidget(), "Z")
		]
		];

	RotationAxisRow.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Rotation axis"))
		];

	/*---------------------------------- Location Axis Content -----------------------------------*/

	FDetailWidgetRow& LocationAxisRow = StructBuilder.AddCustomRow(FText::FromString("Location Axis"));

	LocationAxisRow.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("LocationX")->CreatePropertyValueWidget(), "X")
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("LocationY")->CreatePropertyValueWidget(), "Y")
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BoolWidget(StructPropertyHandle->GetChildHandle("LocationZ")->CreatePropertyValueWidget(), "Z")
		]
		];

	LocationAxisRow.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Location axis"))
		];
}
#endif