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

TSharedRef<SWidget> FDoubleBoneMirrorCustomization::BoolWidget(TSharedRef<SWidget> BoolProperty, FString Text) const
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

TSharedRef<SWidget> FDoubleBoneMirrorCustomization::MakeOption(TSharedPtr<FString> Option) const
{
	return SNew(STextBlock).Text(FText::FromString(*Option));
}

void FDoubleBoneMirrorCustomization::LOnSelection(TSharedPtr<FString> Option, ESelectInfo::Type)
{
	if (!Option.IsValid()) return;
	StructHandle->GetChildHandle("LBoneName")->SetValue(*Option);
	LCurrentSelectItem = Option;
}

void FDoubleBoneMirrorCustomization::ROnSelection(TSharedPtr<FString> Option, ESelectInfo::Type)
{
	if (!Option.IsValid()) return;
	StructHandle->GetChildHandle("RBoneName")->SetValue(*Option);
	RCurrentSelectItem = Option;
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

bool FDoubleBoneMirrorCustomization::CheckSkeletonSelected(UMirrorPoseData*& OuterInstance, USkeleton*& Skeleton) const
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

FReply FDoubleBoneMirrorCustomization::AxisOnClicked() const
{
	if (CurrentDoubleBoneStructure->MirrorAxis == EAxis::Z)
	{
		CurrentDoubleBoneStructure->MirrorAxis = EAxis::None;
	}
	else
	{
		CurrentDoubleBoneStructure->MirrorAxis = StaticCast<EAxis::Type>(StaticCast<int>(CurrentDoubleBoneStructure->MirrorAxis) + 1);
	}

	AxisText->SetText(ConvertAxisFlipTypeToText(CurrentDoubleBoneStructure->MirrorAxis));
	AxisText->SetColorAndOpacity(ConvertAxisFlipTypeToColor(CurrentDoubleBoneStructure->MirrorAxis));
	return FReply::Handled();
}

FReply FDoubleBoneMirrorCustomization::FlipOnClicked() const
{
	if (CurrentDoubleBoneStructure->FlipAxis == EAxis::Z)
	{
		CurrentDoubleBoneStructure->FlipAxis = EAxis::None;
	}
	else
	{
		CurrentDoubleBoneStructure->FlipAxis = StaticCast<EAxis::Type>(StaticCast<int>(CurrentDoubleBoneStructure->FlipAxis) + 1);
	}

	FlipText->SetText(ConvertAxisFlipTypeToText(CurrentDoubleBoneStructure->FlipAxis));
	FlipText->SetColorAndOpacity(ConvertAxisFlipTypeToColor(CurrentDoubleBoneStructure->FlipAxis));
	return FReply::Handled();
}

FLinearColor FDoubleBoneMirrorCustomization::ConvertAxisFlipTypeToColor(const EAxis::Type Type) const
{
	if (Type == EAxis::X)
	{
		return FLinearColor::Red;
	}
	else if (Type == EAxis::Y)
	{
		return FLinearColor::Green;
	}
	else if (Type == EAxis::Z)
	{
		return FLinearColor::Blue;
	}
	

	return FLinearColor::Black;
}

FText FDoubleBoneMirrorCustomization::ConvertAxisFlipTypeToText(const EAxis::Type Type) const
{
	if (Type == EAxis::X)
	{
		return FText::FromString("X");
	}
	else if (Type == EAxis::Y)
	{
		return FText::FromString("Y");
	}
	else if (Type == EAxis::Z)
	{
		return FText::FromString("Z");
	}
	else if (Type == EAxis::None)
	{
		return FText::FromString("0");
	}
	
	return FText::FromString("None - Error");
}

void FDoubleBoneMirrorCustomization::GenerateBoneOptionList(const FReferenceSkeleton RefSkeleton, const FString LCurrentBone, const FString RCurrentBone)
{
	for (int BoneIndex = 0; BoneIndex < RefSkeleton.GetNum(); BoneIndex++)
	{
		TSharedPtr<FString> Item = MakeShareable(new FString(RefSkeleton.GetBoneName(BoneIndex).ToString()));

		if (RefSkeleton.GetBoneName(BoneIndex).ToString() == LCurrentBone)
		{
			LCurrentSelectItem = Item;
		}

		if (RefSkeleton.GetBoneName(BoneIndex).ToString() == RCurrentBone)
		{
			RCurrentSelectItem = Item;
		}

		Options.Add(Item);
	}
}

void FDoubleBoneMirrorCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	TArray<void*> RawData;
	StructHandle->AccessRawData(RawData);

	if (RawData.Num() != 1)
	{
		UE_LOG(LogClass, Error, TEXT("Multiple or zero values on RawData from FDoubleBoneMirrorCustomization::CustomizeHeader"));
		return;
	}
	else
	{
		CurrentDoubleBoneStructure = StaticCast<FDoubleBoneMirror*>(RawData[0]);
		if (CurrentDoubleBoneStructure == nullptr)
		{
			UE_LOG(LogClass, Error, TEXT("Invalid CurrentDoubleBoneStructure"));
			return;
		}
	}
	
	UMirrorPoseData* OuterInstance;
	USkeleton* TargetSkeleton;
	if (!CheckSkeletonSelected(OuterInstance, TargetSkeleton))
	{
		UE_LOG(LogClass, Error, TEXT("Some problem on OuterObjects from FDoubleBoneMirrorCustomization::CheckSkeletonSelected"));
		return;
	}

	const FString LValueContentName = (!CurrentDoubleBoneStructure->LBoneName.IsEmpty() && TargetSkeleton->GetReferenceSkeleton().FindBoneIndex(*CurrentDoubleBoneStructure->LBoneName) != INDEX_NONE) ? CurrentDoubleBoneStructure->LBoneName : "None";
	const FString RValueContentName = (!CurrentDoubleBoneStructure->RBoneName.IsEmpty() && TargetSkeleton->GetReferenceSkeleton().FindBoneIndex(*CurrentDoubleBoneStructure->RBoneName) != INDEX_NONE) ? CurrentDoubleBoneStructure->RBoneName : "None";

	if (CurrentDoubleBoneStructure->MirrorAxis > 3)
	{
		CurrentDoubleBoneStructure->MirrorAxis = EAxis::X;
	}

	if (CurrentDoubleBoneStructure->FlipAxis > 3)
	{
		CurrentDoubleBoneStructure->FlipAxis = EAxis::X;
	}

	GenerateBoneOptionList(TargetSkeleton->GetReferenceSkeleton(), CurrentDoubleBoneStructure->LBoneName, CurrentDoubleBoneStructure->RBoneName);
	
	HeaderRow.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Bone Setting"))
	];

	HeaderRow.ValueContent()
	.MinDesiredWidth(250.0f)
	[
		SNew(SHorizontalBox)
		/*+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(SBox)
			.MinDesiredWidth(200.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("[" + LValueContentName + "]" + " <---> " + "[" + RValueContentName + "]"))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.0f, 0.0f)))
				.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
			]
		]*/
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.MinDesiredWidth(200.0f)
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
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(" <---> "))
			.TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
		[
			SNew(SBox)
			.MinDesiredWidth(200.0f)
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
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Mirror Axis: "))
			.TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
			.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked_Raw(this, &FDoubleBoneMirrorCustomization::AxisOnClicked)
			.ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("ToolBar.Button"))
			[
				SAssignNew(AxisText, STextBlock)
				.Text(ConvertAxisFlipTypeToText(CurrentDoubleBoneStructure->MirrorAxis))
				.ColorAndOpacity(ConvertAxisFlipTypeToColor(CurrentDoubleBoneStructure->MirrorAxis))
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ShadowOffset(FIntPoint(-1, 1))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
			.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Flip Axis: "))
			.TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
			.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked_Raw(this, &FDoubleBoneMirrorCustomization::FlipOnClicked)
			.ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("ToolBar.Button"))
			[
				SAssignNew(FlipText, STextBlock)
				.Text(ConvertAxisFlipTypeToText(CurrentDoubleBoneStructure->FlipAxis))
				.ColorAndOpacity(ConvertAxisFlipTypeToColor(CurrentDoubleBoneStructure->FlipAxis))
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ShadowOffset(FIntPoint(-1, 1))
			]
		]	
	];
}

void FDoubleBoneMirrorCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	return;
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