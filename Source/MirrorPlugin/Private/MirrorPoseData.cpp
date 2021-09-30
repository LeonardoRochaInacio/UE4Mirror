// Fill out your copyright notice in the Description page of Project Settings.

#include "MirrorPoseData.h"
#include "Runtime/Engine/Classes/Animation/AnimSequence.h"
#include "MirrorHelpers.h"

void UMirrorPoseData::ProcessMirrorTrack(const UMirrorPoseData* DataPose, UAnimSequence* AnimSequence)
{
	if (AnimSequence->GetSkeleton() == DataPose->Skeleton)
	{
		TArray<FName> TrackNames = AnimSequence->GetAnimationTrackNames();

		for (FSingleBoneMirror SBoneMirror : DataPose->SingleBones)
		{
			int32 Index = TrackNames.Find(*SBoneMirror.BoneName);
			FRawAnimSequenceTrack& FramePoseTrack = AnimSequence->GetRawAnimationTrack(Index);
			MirrorHelpers::SingleTrackMirror(FramePoseTrack, DataPose->Skeleton->GetRefLocalPoses()[Index], SBoneMirror);
		}


		for (FDoubleBoneMirror DBoneMirror : DataPose->DoubleBones)
		{
			int32 C_L_Index = AnimSequence->GetAnimationTrackNames().Find(*DBoneMirror.LBoneName);
			int32 C_R_Index = AnimSequence->GetAnimationTrackNames().Find(*DBoneMirror.RBoneName);

			int32 L_Index = AnimSequence->GetSkeletonIndexFromRawDataTrackIndex(C_L_Index);
			int32 R_Index = AnimSequence->GetSkeletonIndexFromRawDataTrackIndex(C_R_Index);

			

			FRawAnimSequenceTrack& L_FramePoseTrack = AnimSequence->GetRawAnimationTrack(C_L_Index);
			FRawAnimSequenceTrack& R_FramePoseTrack = AnimSequence->GetRawAnimationTrack(C_R_Index);
			MirrorHelpers::DoubleTrackMirror(L_FramePoseTrack, DataPose->Skeleton->GetRefLocalPoses()[L_Index], R_FramePoseTrack, DataPose->Skeleton->GetRefLocalPoses()[R_Index], DBoneMirror);
		//	UE_LOG(LogClass, Warning, TEXT("%s - %s"), *DataPose->Skeleton->GetReferenceSkeleton().GetBoneName(AnimSequence->GetSkeletonIndexFromRawDataTrackIndex(L_Index)).ToString(), *DataPose->Skeleton->GetReferenceSkeleton().GetBoneName(AnimSequence->GetSkeletonIndexFromRawDataTrackIndex(R_Index)).ToString());
		//	UE_LOG(LogClass, Warning, TEXT("[%i, %i] - [%i, %i]"), C_L_Index, L_Index, C_R_Index, R_Index);
		}

		AnimSequence->PostProcessSequence();
	}
}