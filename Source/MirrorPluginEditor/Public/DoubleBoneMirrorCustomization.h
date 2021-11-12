#pragma once

#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "Runtime/SlateCore/Public/Widgets/SWidget.h"
#include "AnimGraphNode_MirrorPose.h"

class FDoubleBoneMirrorCustomization : public IPropertyTypeCustomization
{

private:

	TSharedRef<SWidget> BoolWidget(TSharedRef<SWidget> BoolProperty, FString Text);

	TSharedRef<SWidget> MakeOption(TSharedPtr<FString> Option);

	FText RGetCurrentItem() const;

	FText LGetCurrentItem() const;

	void ROnSelection(TSharedPtr<FString> Option, ESelectInfo::Type);

	void LOnSelection(TSharedPtr<FString> Option, ESelectInfo::Type);

	bool CheckSkeletonSelected(UMirrorPoseData*& OuterInstance, USkeleton*& Skeleton);

private:

	TArray<TSharedPtr<FString>> Options;

	TSharedPtr<FString> RCurrentSelectItem;

	TSharedPtr<FString> LCurrentSelectItem;

	TSharedPtr<IPropertyHandle> StructHandle;

	IDetailChildrenBuilder* Builder = nullptr;

public:

	/* Interface implementation */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};