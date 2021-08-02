#if WITH_EDITOR

#include "Animation/MirrorPose/IcarusMirrorPoseDataFactory.h"
#include "Animation/MirrorPose/IcarusMirrorPoseData.h"
#include "Developer/AssetTools/Public/AssetTypeCategories.h"

UIcarusMirrorPoseDataFactory::UIcarusMirrorPoseDataFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UIcarusMirrorPoseData::StaticClass();
}

UObject * UIcarusMirrorPoseDataFactory::FactoryCreateNew(UClass * InClass, UObject * InParent, FName InName, EObjectFlags Flags, UObject * Context, FFeedbackContext * Warn)
{
	UIcarusMirrorPoseData* NewAsset = NewObject<UIcarusMirrorPoseData>(InParent, InClass, InName, Flags);
	return NewAsset;
}

uint32 UIcarusMirrorPoseDataFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Animation;
}
#endif
