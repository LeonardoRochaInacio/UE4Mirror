#include "MirrorPlugin/AnimGraphNode_MirrorPose.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"
#include "MirrorPlugin/MirrorHelpers.h"
#include "BonePose.h"

// ---------------------------------------------------------------------------------- 

void FAnimNode_MirrorPose::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	CurrentPose.Initialize(Context);

	SkeletalMesh = Context.AnimInstanceProxy->GetSkelMeshComponent();
}

void FAnimNode_MirrorPose::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	CurrentPose.CacheBones(Context);
}

void FAnimNode_MirrorPose::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	CurrentPose.Update(Context);
	GetEvaluateGraphExposedInputs().Execute(Context);
}

void FAnimNode_MirrorPose::Evaluate_AnyThread(FPoseContext &Context)
{
	CurrentPose.Evaluate(Context);
	
	if (Context.AnimInstanceProxy && SkeletalMesh && bMirrorAnimation)
	{
		for (FSingleBoneMirror SBoneMirror : MirrorPoseAsset->SingleBones)
		{
			FCompactPoseBoneIndex BoneIndex = FCompactPoseBoneIndex(SkeletalMesh->GetBoneIndex(*SBoneMirror.BoneName));
			if (BoneIndex.IsRootBone()) continue;

			FVector BasePoseLocation = Context.Pose.GetRefPose(BoneIndex).GetLocation();
			FVector CurrentPoseLocation = Context.Pose[BoneIndex].GetLocation();
			FRotator BasePoseRotation = Context.Pose.GetRefPose(BoneIndex).GetRotation().Rotator();
			FRotator CurrentPoseRotation = Context.Pose[BoneIndex].GetRotation().Rotator();
			FRotator DifferencePoseRotation = CurrentPoseRotation - BasePoseRotation;
			FVector LocationResult;
			FRotator RotationResult;

			MirrorHelpers::SingleBoneMirror(SBoneMirror, BasePoseLocation, BasePoseRotation, CurrentPoseLocation, CurrentPoseRotation, LocationResult, RotationResult);

			Context.Pose[BoneIndex].SetLocation(LocationResult);
			Context.Pose[BoneIndex].SetRotation(RotationResult.Quaternion());
		}

		
		for (FDoubleBoneMirror DBoneMirror : MirrorPoseAsset->DoubleBones)
		{
			FCompactPoseBoneIndex L_BoneIndex = FCompactPoseBoneIndex(SkeletalMesh->GetBoneIndex(*DBoneMirror.LBoneName));
			FCompactPoseBoneIndex R_BoneIndex = FCompactPoseBoneIndex(SkeletalMesh->GetBoneIndex(*DBoneMirror.RBoneName));
			FRotator L_BasePoseRotation = Context.Pose.GetRefPose(L_BoneIndex).GetRotation().Rotator();
			FRotator R_BasePoseRotation = Context.Pose.GetRefPose(R_BoneIndex).GetRotation().Rotator();
			FRotator L_CurrentPoseRotation = Context.Pose[L_BoneIndex].GetRotation().Rotator();
			FRotator R_CurrentPoseRotation = Context.Pose[R_BoneIndex].GetRotation().Rotator();
			FRotator L_FinalPoseRotation, R_FinalPoseRotation;

			MirrorHelpers::DoubleBoneRotationMirror(DBoneMirror, L_BasePoseRotation, R_BasePoseRotation, L_CurrentPoseRotation, R_CurrentPoseRotation, L_FinalPoseRotation, R_FinalPoseRotation);

			Context.Pose[R_BoneIndex].SetRotation(R_FinalPoseRotation.Quaternion());
			Context.Pose[L_BoneIndex].SetRotation(L_FinalPoseRotation.Quaternion());
		}
	}
};

// ---------------------------------------------------------------------------- 

UAnimGraphNode_MirrorPose::UAnimGraphNode_MirrorPose() {};

FLinearColor UAnimGraphNode_MirrorPose::GetNodeTitleColor() const
{
	return FLinearColor(0.0f, 1.0f, 1.0f);
}

FText UAnimGraphNode_MirrorPose::GetTooltipText() const
{
	return FText();
}

FText UAnimGraphNode_MirrorPose::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("MirrorPose");
}

FText UAnimGraphNode_MirrorPose::GetMenuCategory() const
{
	return FText();
}
