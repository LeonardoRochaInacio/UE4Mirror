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
#include "IPropertyTypeCustomization.h"
#include "MirrorPoseData.h"

TSharedRef<IPropertyTypeCustomization> FSingleBoneMirrorCustomization::MakeInstance()
{
	return MakeShareable(new FSingleBoneMirrorCustomization());
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
		CurrentSingleBoneStructure->MirrorAxis = EAxis::None;
	}
	else
	{
		CurrentSingleBoneStructure->MirrorAxis = StaticCast<EAxis::Type>(StaticCast<int>(CurrentSingleBoneStructure->MirrorAxis) + 1);
	}

	AxisText->SetText(ConvertAxisFlipTypeToText(CurrentSingleBoneStructure->MirrorAxis));
	AxisText->SetColorAndOpacity(ConvertAxisFlipTypeToColor(CurrentSingleBoneStructure->MirrorAxis));
	return FReply::Handled();
}

FReply FSingleBoneMirrorCustomization::FlipOnClicked() const
{	
	if (CurrentSingleBoneStructure->FlipAxis == EAxis::Z)
	{
		CurrentSingleBoneStructure->FlipAxis = EAxis::None;
	}
	else
	{
		CurrentSingleBoneStructure->FlipAxis = StaticCast<EAxis::Type>(StaticCast<int>(CurrentSingleBoneStructure->FlipAxis) + 1);
	}

	FlipText->SetText(ConvertAxisFlipTypeToText(CurrentSingleBoneStructure->FlipAxis));
	FlipText->SetColorAndOpacity(ConvertAxisFlipTypeToColor(CurrentSingleBoneStructure->FlipAxis));
	return FReply::Handled();
}

FLinearColor FSingleBoneMirrorCustomization::ConvertAxisFlipTypeToColor(const EAxis::Type Type) const
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
	else if (Type == EAxis::None)
	{
		return FText::FromString("0");
	}

	return FText::FromString("None - Error");
}



void FSingleBoneMirrorCustomization::GenerateBoneOptionList(const FReferenceSkeleton RefSkeleton, const FString CurrentSelectBone)
{
	for (int BoneIndex = 0; BoneIndex < RefSkeleton.GetNum(); BoneIndex++)
	{
		TSharedPtr<FString> Item = MakeShareable(new FString(RefSkeleton.GetBoneName(BoneIndex).ToString()));
		if (RefSkeleton.GetBoneName(BoneIndex).ToString() == CurrentSelectBone)
		{
			CurrentSelectItem = Item;
		}

		Options.Add(Item);
	}
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
	OuterInstance_Internal = OuterInstance;
	
	const FString ValueContentName = (!CurrentSingleBoneStructure->BoneName.IsEmpty() && 
		TargetSkeleton->GetReferenceSkeleton().FindBoneIndex(*CurrentSingleBoneStructure->BoneName) != INDEX_NONE)
		? CurrentSingleBoneStructure->BoneName : "None";

	if(CurrentSingleBoneStructure->MirrorAxis > 3)
	{
		CurrentSingleBoneStructure->MirrorAxis = EAxis::X;
	}

	if (CurrentSingleBoneStructure->FlipAxis > 3)
	{
		CurrentSingleBoneStructure->FlipAxis = EAxis::X;
	}

	GenerateBoneOptionList(TargetSkeleton->GetReferenceSkeleton(), CurrentSingleBoneStructure->BoneName);


	HeaderRow.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Bone Setting"))
	];

	HeaderRow.ValueContent()
	.MinDesiredWidth(250.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(EVerticalAlignment::VAlign_Center)
		.Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
		[
			SNew(SBox)
			.MinDesiredWidth(200.0f)
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
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
			.Text(FText::FromString("Mirror Axis: "))
			.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked_Raw(this, &FSingleBoneMirrorCustomization::AxisOnClicked)
			.ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("ToolBar.Button"))
			[
				SAssignNew(AxisText, STextBlock)
				.Text(ConvertAxisFlipTypeToText(CurrentSingleBoneStructure->MirrorAxis))
				.ColorAndOpacity(ConvertAxisFlipTypeToColor(CurrentSingleBoneStructure->MirrorAxis))
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ShadowOffset(FIntPoint(-1, 1))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
			.Text(FText::FromString("Flip Axis: "))
			.Margin(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked_Raw(this, &FSingleBoneMirrorCustomization::FlipOnClicked)
			.ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("ToolBar.Button"))
			[
				SAssignNew(FlipText, STextBlock)
				.Text(ConvertAxisFlipTypeToText(CurrentSingleBoneStructure->FlipAxis))
				.ColorAndOpacity(ConvertAxisFlipTypeToColor(CurrentSingleBoneStructure->FlipAxis))
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ShadowOffset(FIntPoint(-1, 1))
			]
		]
	];	
}

void FSingleBoneMirrorCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	return;
}