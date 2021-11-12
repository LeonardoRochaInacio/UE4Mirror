#include "DoubleBoneMirrorCustomization.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Editor/PropertyEditor/Public/IDetailChildrenBuilder.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"
#include "Runtime/SlateCore/Public/Widgets/SBoxPanel.h"
#include "PropertyCustomizationHelpers.h"
#include "Runtime/UMG/Public/Components/ComboBoxString.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "MirrorPoseData.h"
#include "Widgets/Input/SSlider.h"


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
			+ SHorizontalBox::Slot()
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