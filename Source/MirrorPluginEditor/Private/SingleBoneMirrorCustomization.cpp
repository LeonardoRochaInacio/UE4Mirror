#include "SingleBoneMirrorCustomization.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Editor/PropertyEditor/Public/IDetailChildrenBuilder.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"
#include "Animation/AnimInstance.h"
#include "Runtime/SlateCore/Public/Widgets/SBoxPanel.h"
#include "PropertyCustomizationHelpers.h"
#include "Runtime/UMG/Public/Components/ComboBoxString.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "MirrorPoseData.h"

TSharedRef<IPropertyTypeCustomization> FSingleBoneMirrorCustomization::MakeInstance()
{
	return MakeShareable(new FSingleBoneMirrorCustomization());
}

TSharedRef<SWidget> FSingleBoneMirrorCustomization::BoolWidget(const TSharedRef<SWidget> BoolProperty, const FString Text) const
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

TSharedRef<SWidget> FSingleBoneMirrorCustomization::MakeOption(const TSharedPtr<FString> Option) const
{
	return SNew(STextBlock).Text(FText::FromString(*Option));
}

void FSingleBoneMirrorCustomization::OnSelection(const TSharedPtr<FString> Option, ESelectInfo::Type)
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

bool FSingleBoneMirrorCustomization::CheckSkeletonSelected(UMirrorPoseData*& OuterInstance, USkeleton*& Skeleton) const
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

FReply FSingleBoneMirrorCustomization::AxisOnClicked() const
{
	if (CurrentSingleBoneStructure->MirrorAxis == EAxis::Z)
	{
		CurrentSingleBoneStructure->MirrorAxis = EAxis::X;
	}
	else
	{
		CurrentSingleBoneStructure->MirrorAxis = StaticCast<EAxis::Type>(StaticCast<int>(CurrentSingleBoneStructure->MirrorAxis) + 1);
	}

	AxisText->SetText(ConvertAxisFlipTypeToText(CurrentSingleBoneStructure->MirrorAxis));

	return FReply::Handled();
}

FReply FSingleBoneMirrorCustomization::FlipOnClicked() const
{	
	if (CurrentSingleBoneStructure->FlipAxis == EAxis::Z)
	{
		CurrentSingleBoneStructure->FlipAxis = EAxis::X;
	}
	else
	{
		CurrentSingleBoneStructure->FlipAxis = StaticCast<EAxis::Type>(StaticCast<int>(CurrentSingleBoneStructure->FlipAxis) + 1);
	}

	FlipText->SetText(ConvertAxisFlipTypeToText(CurrentSingleBoneStructure->FlipAxis));

	return FReply::Handled();
}

FText FSingleBoneMirrorCustomization::ConvertAxisFlipTypeToText(const EAxis::Type Type) const
{
	if(Type == EAxis::X)
	{
		return FText::FromString("X");
	}
	else if(Type == EAxis::Y)
	{
		return FText::FromString("Y");
	}
	else if(Type == EAxis::Z)
	{
		return FText::FromString("Z");
	}

	return FText::FromString("None - Error");
}

void FSingleBoneMirrorCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	TArray<void*> RawData;
	StructHandle->AccessRawData(RawData);
	
	if (RawData.Num() != 1)
	{
		UE_LOG(LogClass, Error, TEXT("Multiple or zero values on RawData from FSingleBoneMirrorCustomization::CustomizeHeader"));
		return;
	}
	else
	{
		CurrentSingleBoneStructure = StaticCast<FSingleBoneMirror*>(RawData[0]);
		if(CurrentSingleBoneStructure == nullptr)
		{
			UE_LOG(LogClass, Error, TEXT("Invalid CurrentSingleBoneStructure"));
			return;
		}
	}
	
	UMirrorPoseData* OuterInstance;
	USkeleton* TargetSkeleton;
	if (!CheckSkeletonSelected(OuterInstance, TargetSkeleton))
	{
		UE_LOG(LogClass, Error, TEXT("Some problem on OuterObjects from FSingleBoneMirrorCustomization::CheckSkeletonSelected"));
		return;
	}

	FString CurrentSelectBone;
	StructHandle->GetChildHandle("BoneName")->GetValue(CurrentSelectBone);
	
	const FString ValueContentName = (!CurrentSelectBone.IsEmpty() && TargetSkeleton->GetReferenceSkeleton().FindBoneIndex(*CurrentSelectBone) != INDEX_NONE) ? CurrentSelectBone : "None";

	if(CurrentSingleBoneStructure->MirrorAxis == 0 || CurrentSingleBoneStructure->MirrorAxis > 3)
	{
		CurrentSingleBoneStructure->MirrorAxis = EAxis::X;
	}

	if (CurrentSingleBoneStructure->FlipAxis == 0 || CurrentSingleBoneStructure->FlipAxis > 3)
	{
		CurrentSingleBoneStructure->FlipAxis = EAxis::X;
	}
	
	HeaderRow.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Bone Settings"))
	];

	HeaderRow.ValueContent()
	.MinDesiredWidth(250.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(SBox)
			.MinDesiredWidth(200.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("[" + ValueContentName + "]"))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.0f, 0.0f)))
				.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Mirror Axis: "))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.0f, 0.0f)))
			.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
		.AutoWidth()
		[
			SNew(SButton).OnClicked_Raw(this, &FSingleBoneMirrorCustomization::AxisOnClicked)
			[
				SAssignNew(AxisText, STextBlock).Text(ConvertAxisFlipTypeToText(CurrentSingleBoneStructure->MirrorAxis))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Flip Axis: "))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.0f, 0.0f)))
			.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
		.AutoWidth()
		[
			SNew(SButton).OnClicked_Raw(this, &FSingleBoneMirrorCustomization::FlipOnClicked)
			[
				SAssignNew(FlipText, STextBlock).Text(ConvertAxisFlipTypeToText(CurrentSingleBoneStructure->FlipAxis))
			]
		]
	];	
}

void FSingleBoneMirrorCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	return;
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