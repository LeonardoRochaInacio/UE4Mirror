#if WITH_EDITOR
#pragma once

#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "Editor/PropertyEditor/Public/IDetailCustomization.h"
#include "Runtime/SlateCore/Public/Widgets/SWidget.h"
#include "AssetTypeActions_Base.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"
#include "AnimGraphNode_MirrorPose.h"



class FIcarusMirrorPoseDataDetails : public IDetailCustomization
{
public:
	
	TSharedPtr<FString> CurrentSelectItem;

	TArray<TSharedPtr<FString>> Options;

	IDetailLayoutBuilder* Builder;

	TSharedRef<SWidget> MakeOption(TSharedPtr<FString> Option);

	FText GetCurrentItem() const;

	void OnSelection(TSharedPtr<FString> Option, ESelectInfo::Type);

	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

/* --------------------------------------------------------------------------------------------- */

class FSingleBoneMirrorCustomization : public IPropertyTypeCustomization
{

private:

	TSharedRef<SWidget> BoolWidget(TSharedRef<SWidget> BoolProperty, FString Text);

	TSharedRef<SWidget> MakeOption(TSharedPtr<FString> Option);

	FText GetCurrentItem() const;

	void OnSelection(TSharedPtr<FString> Option, ESelectInfo::Type);

	bool CheckSkeletonSelected(UIcarusMirrorPoseData*& OuterInstance, USkeleton*& Skeleton);

private:

	TArray<TSharedPtr<FString>> Options;

	TSharedPtr<FString> CurrentSelectItem;

	TSharedPtr<IPropertyHandle> StructHandle;

	IDetailChildrenBuilder* Builder;

public:

	/* Interface implementation */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};

/* --------------------------------------------------------------------------------------------- */

class FDoubleBoneMirrorCustomization : public IPropertyTypeCustomization
{

private:

	TSharedRef<SWidget> BoolWidget(TSharedRef<SWidget> BoolProperty, FString Text);

	TSharedRef<SWidget> MakeOption(TSharedPtr<FString> Option);

	FText RGetCurrentItem() const;

	FText LGetCurrentItem() const;

	void ROnSelection(TSharedPtr<FString> Option, ESelectInfo::Type);

	void LOnSelection(TSharedPtr<FString> Option, ESelectInfo::Type);

	bool CheckSkeletonSelected(UIcarusMirrorPoseData*& OuterInstance, USkeleton*& Skeleton);

private:

	TArray<TSharedPtr<FString>> Options;

	TSharedPtr<FString> RCurrentSelectItem;

	TSharedPtr<FString> LCurrentSelectItem;

	TSharedPtr<IPropertyHandle> StructHandle;

	IDetailChildrenBuilder* Builder;

public:

	/* Interface implementation */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};
#endif