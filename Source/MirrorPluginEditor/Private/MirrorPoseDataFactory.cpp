#include "MirrorPoseDataFactory.h"
#include "MirrorPoseData.h"
#include "Developer/AssetTools/Public/AssetTypeCategories.h"

UMirrorPoseDataFactory::UMirrorPoseDataFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UMirrorPoseData::StaticClass();
}

UObject * UMirrorPoseDataFactory::FactoryCreateNew(UClass * InClass, UObject * InParent, FName InName, EObjectFlags Flags, UObject * Context, FFeedbackContext * Warn)
{
	UMirrorPoseData* NewAsset = NewObject<UMirrorPoseData>(InParent, InClass, InName, Flags);
	return NewAsset;
}

uint32 UMirrorPoseDataFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Animation;
}