#pragma once

#include "AnimGraphNode_Base.h"
#include "Runtime/UMG/Public/Components/ComboBoxString.h"
#include "IcarusMirrorPoseData.h"
#include "CoreUObject.h"
#include "AnimGraphNode_MirrorPose.generated.h"

USTRUCT(BlueprintType)
struct ICARUSEDITOR_API FAnimNode_MirrorPose : public FAnimNode_Base
{

	GENERATED_USTRUCT_BODY()

public:

	FAnimNode_MirrorPose() {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Links)
	FPoseLink CurrentPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	UIcarusMirrorPoseData* MirrorPoseAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	bool bMirrorRootBoneTranslation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	bool bMirrorAnimation = true;

private:

	USkeletalMeshComponent* SkeletalMesh;

	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext &Context) override;

};

// ---------------------------------------------------------------------------- 

UCLASS(BlueprintType)
class ICARUSEDITOR_API UAnimGraphNode_MirrorPose : public UAnimGraphNode_Base
{
	GENERATED_BODY()

public:

	UAnimGraphNode_MirrorPose();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mirror)
		FAnimNode_MirrorPose RawAnimation;

	virtual FLinearColor GetNodeTitleColor() const;
	virtual FText GetTooltipText() const;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const;
	virtual FText GetMenuCategory() const;
};

