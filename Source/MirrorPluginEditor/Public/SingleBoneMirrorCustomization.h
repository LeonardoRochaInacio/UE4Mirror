#pragma once

#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "Editor/PropertyEditor/Public/IDetailCustomization.h"
#include "Runtime/SlateCore/Public/Widgets/SWidget.h"

class UMirrorPoseData;
class USkeleton;

class FSingleBoneMirrorCustomization : public IPropertyTypeCustomization
{

private:

	TSharedRef<SWidget> BoolWidget(const TSharedRef<SWidget> BoolProperty, const FString Text) const;

	TSharedRef<SWidget> MakeOption(const TSharedPtr<FString> Option) const;

	FText GetCurrentItem() const;

	void OnSelection(TSharedPtr<FString> Option, ESelectInfo::Type);

	bool CheckSkeletonSelected(UMirrorPoseData*& OuterInstance, USkeleton*& Skeleton) const;

private:

	TArray<TSharedPtr<FString>> Options;

	TSharedPtr<FString> CurrentSelectItem;

	TSharedPtr<IPropertyHandle> StructHandle;

	IDetailChildrenBuilder* Builder = nullptr;
	
	FReply AxisOnClicked() const;

	FReply FlipOnClicked() const;

	FText ConvertAxisFlipTypeToText(const EAxis::Type Type) const;

	TSharedPtr<STextBlock> AxisText;

	TSharedPtr<STextBlock> FlipText;

	struct FSingleBoneMirror* CurrentSingleBoneStructure = nullptr;
	
public:

	/* Interface implementation */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};
