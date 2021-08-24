// Fill out your copyright notice in the Description page of Project Settings.

#include "MirrorPlugin/MirrorHelpers.h"

void MirrorHelpers::SingleBoneMirror(FSingleBoneMirror BoneRules, FVector BasePoseLocation, FRotator BasePoseRotation, FVector FramePoseLocation, FRotator FramePoseRotation, FVector & LocationResult, FRotator & RotationResult)
{
	FramePoseLocation.X = (BoneRules.LocationX) ? FramePoseLocation.X*-1.0f : FramePoseLocation.X;
	FramePoseLocation.Y = (BoneRules.LocationY) ? FramePoseLocation.Y*-1.0f : FramePoseLocation.Y;
	FramePoseLocation.Z = (BoneRules.LocationZ) ? FramePoseLocation.Z*-1.0f : FramePoseLocation.Z;

	LocationResult = FramePoseLocation;

	FRotator DifferencePoseRotation = FramePoseRotation - BasePoseRotation;
	DifferencePoseRotation.Roll = (BoneRules.RotationX) ? DifferencePoseRotation.Roll*-1.0f : DifferencePoseRotation.Roll;
	DifferencePoseRotation.Pitch = (BoneRules.RotationY) ? DifferencePoseRotation.Pitch*-1.0f : DifferencePoseRotation.Pitch;
	DifferencePoseRotation.Yaw = (BoneRules.RotationZ) ? DifferencePoseRotation.Yaw*-1.0f : DifferencePoseRotation.Yaw;

	RotationResult = DifferencePoseRotation + BasePoseRotation;
}

void MirrorHelpers::DoubleBoneRotationMirror(FDoubleBoneMirror BoneRules, FRotator L_BasePoseRotation, FRotator R_BasePoseRotation, FRotator L_FramePoseRotation, FRotator R_FramePoseRotation, FRotator & L_RotationResult, FRotator & R_RotationResult)
{
	FRotator L_DifferencePoseRotation = L_FramePoseRotation - L_BasePoseRotation;
	FRotator R_DifferencePoseRotation = R_FramePoseRotation - R_BasePoseRotation;

	R_DifferencePoseRotation.Roll = (BoneRules.RotationX) ? R_DifferencePoseRotation.Roll*-1.0f : R_DifferencePoseRotation.Roll;
	R_DifferencePoseRotation.Pitch = (BoneRules.RotationY) ? R_DifferencePoseRotation.Pitch*-1.0f : R_DifferencePoseRotation.Pitch;
	R_DifferencePoseRotation.Yaw = (BoneRules.RotationZ) ? R_DifferencePoseRotation.Yaw*-1.0f : R_DifferencePoseRotation.Yaw;

	L_DifferencePoseRotation.Roll = (BoneRules.RotationX) ? L_DifferencePoseRotation.Roll*-1.0f : L_DifferencePoseRotation.Roll;
	L_DifferencePoseRotation.Pitch = (BoneRules.RotationY) ? L_DifferencePoseRotation.Pitch*-1.0f : L_DifferencePoseRotation.Pitch;
	L_DifferencePoseRotation.Yaw = (BoneRules.RotationZ) ? L_DifferencePoseRotation.Yaw*-1.0f : L_DifferencePoseRotation.Yaw;

	L_RotationResult = R_DifferencePoseRotation + L_BasePoseRotation;
	R_RotationResult = L_DifferencePoseRotation + R_BasePoseRotation;

}

void MirrorHelpers::SingleTrackMirror(FRawAnimSequenceTrack& Track, FTransform BaseBoneTransform, FSingleBoneMirror TrackRules)
{
	for (int ip = 0; ip < Track.PosKeys.Num(); ip++)
	{
		Track.PosKeys[ip].X = (TrackRules.LocationX) ? Track.PosKeys[ip].X*-1.0f : Track.PosKeys[ip].X;
		Track.PosKeys[ip].Y = (TrackRules.LocationY) ? Track.PosKeys[ip].Y*-1.0f : Track.PosKeys[ip].Y;
		Track.PosKeys[ip].Z = (TrackRules.LocationZ) ? Track.PosKeys[ip].Z*-1.0f : Track.PosKeys[ip].Z;
	}

	for (int ir = 0; ir < Track.RotKeys.Num(); ir++)
	{
		FRotator BasePoseRotation = BaseBoneTransform.GetRotation().Rotator();
		FRotator FramePoseRotation = Track.RotKeys[ir].Rotator();
		FRotator DifferencePoseRotation = FramePoseRotation - BasePoseRotation;

		DifferencePoseRotation.Roll = (TrackRules.RotationX) ? DifferencePoseRotation.Roll*-1.0f : DifferencePoseRotation.Roll;
		DifferencePoseRotation.Pitch = (TrackRules.RotationY) ? DifferencePoseRotation.Pitch*-1.0f : DifferencePoseRotation.Pitch;
		DifferencePoseRotation.Yaw = (TrackRules.RotationZ) ? DifferencePoseRotation.Yaw*-1.0f : DifferencePoseRotation.Yaw;

		Track.RotKeys[ir] = (DifferencePoseRotation + BasePoseRotation).Quaternion();
	}
}

void MirrorHelpers::DoubleTrackMirror(FRawAnimSequenceTrack& L_Track, FTransform L_BaseBoneTransform, FRawAnimSequenceTrack& R_Track, FTransform R_BaseBoneTransform, FDoubleBoneMirror TrackRules)
{
	int32 L_Track_Num = L_Track.RotKeys.Num();
	int32 R_Track_Num = R_Track.RotKeys.Num();

	FQuat Rot_R_Last_Key = R_Track.RotKeys[R_Track_Num - 1];
	FQuat Rot_L_Last_Key = L_Track.RotKeys[L_Track_Num - 1];

	if (L_Track_Num != R_Track_Num)
	{
		if (L_Track_Num > R_Track_Num)
		{
			int KeysDifference = L_Track_Num - R_Track_Num;
			for (int i = 0; i < KeysDifference; i++)
			{
				R_Track.RotKeys.Add(Rot_R_Last_Key);
			}
		}
		else
		{
			int KeysDifference = R_Track_Num - L_Track_Num;
			for (int i = 0; i < KeysDifference; i++)
			{
				L_Track.RotKeys.Add(Rot_L_Last_Key);
			}
		}
	}

	L_Track_Num = L_Track.RotKeys.Num();
	R_Track_Num = R_Track.RotKeys.Num();

	for (int ir = 0; ir < L_Track.RotKeys.Num(); ir++)
	{
		if (ir + 1 > FMath::Min<int32>(L_Track_Num, R_Track_Num)) { UE_LOG(LogClass, Warning, TEXT("DoubleTrackMirror: Number keys of %s e %s bones dont same"), *TrackRules.LBoneName, *TrackRules.RBoneName); return; }
		FRotator L_BasePoseRotation = L_BaseBoneTransform.Rotator();
		FRotator R_BasePoseRotation = R_BaseBoneTransform.Rotator();
		FRotator L_CurrentPoseRotation = L_Track.RotKeys[ir].Rotator();
		FRotator R_CurrentPoseRotation = R_Track.RotKeys[ir].Rotator();
		FRotator L_FinalPoseRotation, R_FinalPoseRotation;


		MirrorHelpers::DoubleBoneRotationMirror(TrackRules, L_BasePoseRotation, R_BasePoseRotation, L_CurrentPoseRotation, R_CurrentPoseRotation, L_FinalPoseRotation, R_FinalPoseRotation);
	
		L_Track.RotKeys[ir] = L_FinalPoseRotation.Quaternion();
		R_Track.RotKeys[ir] = R_FinalPoseRotation.Quaternion();
	}
}