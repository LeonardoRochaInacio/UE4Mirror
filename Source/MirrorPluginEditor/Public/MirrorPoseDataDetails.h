#pragma once

#include "Editor/PropertyEditor/Public/IDetailCustomization.h"
#include "Runtime/SlateCore/Public/Widgets/SWidget.h"
#include "AnimGraphNode_MirrorPose.h"
#include "SPlaneViewport.h"



class MIRRORPLUGINEDITOR_API FMirrorPoseDataDetails final : public IDetailCustomization
{
public:

	TSharedPtr<FString> CurrentSelectItem;

	TArray<TSharedPtr<FString>> Options;

	IDetailLayoutBuilder* Builder;

	TSharedRef<SWidget> MakeOption(const TSharedPtr<FString> Option) const;

	FText GetCurrentItem() const;

	void OnSelection(const TSharedPtr<FString> Option, ESelectInfo::Type);

	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	void GenerateViewportPlane(IDetailLayoutBuilder& DetailBuilder, USkeleton* SelectedSkeleton);

	TSharedRef<SBox> GenerateViewportPlaneControls() const;

	void ViewportPlaneControl_SliderAction(float NewValue) const;

	TSharedPtr<SPlaneViewport> PlaneViewport;
};
