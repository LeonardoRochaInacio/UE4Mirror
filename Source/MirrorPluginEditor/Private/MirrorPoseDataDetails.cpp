#include "MirrorPoseDataDetails.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Editor/PropertyEditor/Public/IDetailPropertyRow.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"
#include "Runtime/SlateCore/Public/Widgets/SBoxPanel.h"
#include "PropertyCustomizationHelpers.h"
#include "Runtime/UMG/Public/Components/ComboBoxString.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "MirrorPoseData.h"
#include "Widgets/Input/SSpinBox.h"

TSharedRef<IDetailCustomization> FMirrorPoseDataDetails::MakeInstance()
{
	return MakeShareable(new FMirrorPoseDataDetails);
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


TSharedRef<SBox> FMirrorPoseDataDetails::GenerateViewportPlaneControls() const
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
			SNew(SSpinBox<float>).MaxValue(50.0f
			).MinValue(0.0f).MinDesiredWidth(100.0f)
		]
		]
		];
}

void FMirrorPoseDataDetails::ViewportPlaneControl_SliderAction(float NewValue) const
{
	if (PlaneViewport.IsValid())
	{
		PlaneViewport->GetVisualMirrorPlane()->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f) + FRotator(0.0f, NewValue, 0.0f));
	}

}

void FMirrorPoseDataDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	UObject* SelectedSkeleton;
	DetailBuilder.GetProperty("Skeleton")->GetValue(SelectedSkeleton);
	DetailBuilder.GetProperty("Skeleton")->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([&]() { DetailBuilder.ForceRefreshDetails(); }));

	if (SelectedSkeleton)
	{
		//GenerateViewportPlane(DetailBuilder, Cast<USkeleton>(SelectedSkeleton));
		USkeleton* Skeleton = Cast<USkeleton>(SelectedSkeleton);
		IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory("GeneralSettings");

		CategoryBuilder.AddProperty("SingleBones");
		CategoryBuilder.AddProperty("DoubleBones");
	}
	else
	{
		DetailBuilder.HideCategory("GeneralSettings");
	}
}