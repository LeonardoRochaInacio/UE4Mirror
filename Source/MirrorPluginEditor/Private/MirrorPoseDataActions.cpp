#include "MirrorPoseDataActions.h"
#include "MirrorPoseData.h"


FColor FMirrorPoseDataActions::GetTypeColor() const
{
	return FColor::Black;
}

FText FMirrorPoseDataActions::GetName() const
{
	return FText::FromName(TEXT("MirrorPoseTable"));
}

UClass* FMirrorPoseDataActions::GetSupportedClass() const
{
	return UMirrorPoseData::StaticClass();
}

uint32 FMirrorPoseDataActions::GetCategories()
{
	return EAssetTypeCategories::Animation;
}