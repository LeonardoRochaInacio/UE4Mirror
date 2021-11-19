// Fill out your copyright notice in the Description page of Project Settings.

#include "MirrorHelpers.h"

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

bool MirrorHelpers::DoubleBoneMirror(UAnimSequence* SequenceToMirror, FDoubleBoneMirror TrackRules, 
	FRawAnimSequenceTrack& Left_Mirrored, FRawAnimSequenceTrack& Right_Mirrored)
{
	if (!SequenceToMirror || !SequenceToMirror->GetSkeleton()) return false;
	const FReferenceSkeleton& Skeleton = SequenceToMirror->GetSkeleton()->GetReferenceSkeleton();
	if (Skeleton.FindBoneIndex(FName(*TrackRules.RBoneName)) == -1 || Skeleton.FindBoneIndex(FName(*TrackRules.LBoneName)) == -1) return false;
	const int FrameNumber = SequenceToMirror->GetNumberOfFrames();
	const TArray<FName> TrackNames = SequenceToMirror->GetAnimationTrackNames();
	const int Left_TrackIndex = TrackNames.IndexOfByKey(FName(*TrackRules.LBoneName));
	const int Right_TrackIndex = TrackNames.IndexOfByKey(FName(*TrackRules.RBoneName));
	if (Left_TrackIndex == -1 && Right_TrackIndex == -1) return false;
	
	TArray<FVector> Left_MirroredPositionKeys;
	TArray<FQuat> Left_MirroredRotationKeys;
	TArray<FVector> Left_MirroredScaleKeys;
	
	Left_MirroredPositionKeys.Reserve(FrameNumber);
	Left_MirroredRotationKeys.Reserve(FrameNumber);
	Left_MirroredScaleKeys.Reserve(FrameNumber);

	TArray<FVector> Right_MirroredPositionKeys;
	TArray<FQuat> Right_MirroredRotationKeys;
	TArray<FVector> Right_MirroredScaleKeys;

	Right_MirroredPositionKeys.Reserve(FrameNumber);
	Right_MirroredRotationKeys.Reserve(FrameNumber);
	Right_MirroredScaleKeys.Reserve(FrameNumber);

	FRawAnimSequenceTrack Left_RawTrack = SequenceToMirror->GetRawAnimationData()[Left_TrackIndex];
	FRawAnimSequenceTrack Right_RawTrack = SequenceToMirror->GetRawAnimationData()[Right_TrackIndex];

	FTransform ReferencePose_Left = Skeleton.GetRefBonePose()[Skeleton.FindBoneIndex(FName(*TrackRules.LBoneName))];
	FTransform ReferencePose_Right = Skeleton.GetRefBonePose()[Skeleton.FindBoneIndex(FName(*TrackRules.RBoneName))];
	FRotator Reference_RMinusL = ReferencePose_Right.Rotator();
	FRotator Reference_LMinusR = ReferencePose_Left.Rotator();
	
	Reference_RMinusL.Roll -= ReferencePose_Left.Rotator().Roll;
	Reference_RMinusL.Pitch -= ReferencePose_Left.Rotator().Pitch;
	Reference_RMinusL.Yaw -= ReferencePose_Left.Rotator().Yaw;

	Reference_LMinusR.Roll -= ReferencePose_Right.Rotator().Roll;
	Reference_LMinusR.Pitch -= ReferencePose_Right.Rotator().Pitch;
	Reference_LMinusR.Yaw -= ReferencePose_Right.Rotator().Yaw;
	
	if(Left_TrackIndex >= 0)
	{
		for (int FrameIteration = 0; FrameIteration < FrameNumber; FrameIteration++)
		{
			const bool bShouldSetPosition = Left_RawTrack.PosKeys.IsValidIndex(FrameIteration);
			const bool bShouldSetRotation = Left_RawTrack.RotKeys.IsValidIndex(FrameIteration);
			const bool bShouldSetScale = Left_RawTrack.ScaleKeys.IsValidIndex(FrameIteration);

			FTransform OriginalSide_Transform;

			if (bShouldSetPosition) OriginalSide_Transform.SetTranslation(Left_RawTrack.PosKeys[FrameIteration]);
			if (bShouldSetRotation) OriginalSide_Transform.SetRotation(Left_RawTrack.RotKeys[FrameIteration]);
			if (bShouldSetScale) OriginalSide_Transform.SetScale3D(Left_RawTrack.ScaleKeys[FrameIteration]);
			
			UE_LOG(LogClass, Warning, TEXT("[R SIDE] Before mirror frame %i from bone %s with rotation %s."),
			FrameIteration, *TrackRules.LBoneName, *OriginalSide_Transform.Rotator().ToString());

			OriginalSide_Transform.Mirror(TrackRules.MirrorAxis, TrackRules.FlipAxis);

			FRotator Temp_OriginalSideRotator = OriginalSide_Transform.Rotator();
			Temp_OriginalSideRotator.Roll += Reference_RMinusL.Roll;
			Temp_OriginalSideRotator.Pitch += Reference_RMinusL.Pitch;
			Temp_OriginalSideRotator.Yaw += Reference_RMinusL.Yaw;
			OriginalSide_Transform.SetRotation(Temp_OriginalSideRotator.Quaternion());
			OriginalSide_Transform.NormalizeRotation();
			
			UE_LOG(LogClass, Warning, TEXT("[R SIDE] After mirror frame %i from bone %s with rotation %s."),
			FrameIteration, *TrackRules.LBoneName, *OriginalSide_Transform.Rotator().ToString());
			
			//OriginalSide_Transform.SetRotation(OriginalSide_Transform.GetRotation() + TrackRules.RotationDifference.Quaternion());
			//OriginalSide_Transform.NormalizeRotation();
			OriginalSide_Transform.SetScale3D(OriginalSide_Transform.GetScale3D().GetAbs());

			if (bShouldSetPosition) Left_MirroredPositionKeys.Add(OriginalSide_Transform.GetTranslation());
			if (bShouldSetRotation) Left_MirroredRotationKeys.Add(OriginalSide_Transform.GetRotation());
			if (bShouldSetScale) Left_MirroredScaleKeys.Add(OriginalSide_Transform.GetScale3D());
		}
	}
	else
	{
		FTransform ReferencePoseTransform_Left = Skeleton.GetRefBonePose()[Skeleton.FindBoneIndex(FName(*TrackRules.LBoneName))];
		ReferencePoseTransform_Left.Mirror(TrackRules.MirrorAxis, TrackRules.FlipAxis);
		//ReferencePoseTransform_Left.SetRotation(ReferencePoseTransform_Left.GetRotation() + TrackRules.RotationDifference.Quaternion());
		//ReferencePoseTransform_Left.NormalizeRotation();
		ReferencePoseTransform_Left.SetScale3D(ReferencePoseTransform_Left.GetScale3D().GetAbs());

		Left_MirroredPositionKeys.Add(ReferencePoseTransform_Left.GetTranslation());
		Left_MirroredRotationKeys.Add(ReferencePoseTransform_Left.GetRotation());
	}
	
	
	if (Right_TrackIndex >= 0)
	{
		for (int FrameIteration = 0; FrameIteration < FrameNumber; FrameIteration++)
		{
			const bool bShouldSetPosition = Right_RawTrack.PosKeys.IsValidIndex(FrameIteration);
			const bool bShouldSetRotation = Right_RawTrack.RotKeys.IsValidIndex(FrameIteration);
			const bool bShouldSetScale = Right_RawTrack.ScaleKeys.IsValidIndex(FrameIteration);

			FTransform OtherSide_Transform;

			if (bShouldSetPosition) OtherSide_Transform.SetTranslation(Right_RawTrack.PosKeys[FrameIteration]);
			if (bShouldSetRotation) OtherSide_Transform.SetRotation(Right_RawTrack.RotKeys[FrameIteration]);
			if (bShouldSetScale) OtherSide_Transform.SetScale3D(Right_RawTrack.ScaleKeys[FrameIteration]);
			
			UE_LOG(LogClass, Warning, TEXT("[L SIDE] Before mirror frame %i from bone %s with rotation %s."),
			FrameIteration, *TrackRules.LBoneName, *OtherSide_Transform.Rotator().ToString());
			
			OtherSide_Transform.Mirror(TrackRules.MirrorAxis, TrackRules.FlipAxis);

			FRotator Temp_OtherSideRotator = OtherSide_Transform.Rotator();
			Temp_OtherSideRotator.Roll += Reference_LMinusR.Roll;
			Temp_OtherSideRotator.Pitch += Reference_LMinusR.Pitch;
			Temp_OtherSideRotator.Yaw += Reference_LMinusR.Yaw;
			OtherSide_Transform.SetRotation(Temp_OtherSideRotator.Quaternion());
			OtherSide_Transform.NormalizeRotation();
			
			UE_LOG(LogClass, Warning, TEXT("[L SIDE] After mirror frame %i from bone %s with rotation %s."),
			FrameIteration, *TrackRules.LBoneName, *OtherSide_Transform.Rotator().ToString());
			
			//OtherSide_Transform.SetRotation(OtherSide_Transform.GetRotation() + TrackRules.RotationDifference.Quaternion());
			//OtherSide_Transform.NormalizeRotation();
			OtherSide_Transform.SetScale3D(OtherSide_Transform.GetScale3D().GetAbs());

			if (bShouldSetPosition) Right_MirroredPositionKeys.Add(OtherSide_Transform.GetTranslation());
			if (bShouldSetRotation) Right_MirroredRotationKeys.Add(OtherSide_Transform.GetRotation());
			if (bShouldSetScale) Right_MirroredScaleKeys.Add(OtherSide_Transform.GetScale3D());
		}
	}
	else
	{
		FTransform ReferencePoseTransform_Right = Skeleton.GetRefBonePose()[Skeleton.FindBoneIndex(FName(*TrackRules.RBoneName))];
		ReferencePoseTransform_Right.Mirror(TrackRules.MirrorAxis, TrackRules.FlipAxis);
		//ReferencePoseTransform_Right.SetRotation(ReferencePoseTransform_Right.GetRotation() + TrackRules.RotationDifference.Quaternion());
		//ReferencePoseTransform_Right.NormalizeRotation();
		ReferencePoseTransform_Right.SetScale3D(ReferencePoseTransform_Right.GetScale3D().GetAbs());

		Right_MirroredPositionKeys.Add(ReferencePoseTransform_Right.GetTranslation());
		Right_MirroredRotationKeys.Add(ReferencePoseTransform_Right.GetRotation());
	}
	

	Left_Mirrored.PosKeys = TrackRules.bShouldMirrorTranslation ? Right_MirroredPositionKeys : Left_MirroredPositionKeys;
	Left_Mirrored.RotKeys = Left_MirroredRotationKeys;
	Left_Mirrored.ScaleKeys = Left_MirroredScaleKeys;

	Right_Mirrored.PosKeys = TrackRules.bShouldMirrorTranslation ? Left_MirroredPositionKeys : Right_MirroredPositionKeys;
	Right_Mirrored.RotKeys = Right_MirroredRotationKeys;
	Right_Mirrored.ScaleKeys = Right_MirroredScaleKeys;

	return true;
}

bool MirrorHelpers::SingleBoneMirror(UAnimSequence* SequenceToMirror, FSingleBoneMirror TrackRules,
	FRawAnimSequenceTrack& Mirrored)
{
	if (!SequenceToMirror || !SequenceToMirror->GetSkeleton()) return false;
	const FReferenceSkeleton& Skeleton = SequenceToMirror->GetSkeleton()->GetReferenceSkeleton();
	if (Skeleton.FindBoneIndex(FName(*TrackRules.BoneName)) == -1) return false;
	const int FrameNumber = SequenceToMirror->GetNumberOfFrames();
	const TArray<FName> TrackNames = SequenceToMirror->GetAnimationTrackNames();
	const int TrackIndex = TrackNames.IndexOfByKey(FName(*TrackRules.BoneName));
	if (TrackIndex == -1) return false;

	TArray<FVector> MirroredPositionKeys;
	TArray<FQuat> MirroredRotationKeys;
	TArray<FVector> MirroredScaleKeys;

	MirroredPositionKeys.Reserve(FrameNumber);
	MirroredRotationKeys.Reserve(FrameNumber);
	MirroredScaleKeys.Reserve(FrameNumber);

	FRawAnimSequenceTrack RawTrack = SequenceToMirror->GetRawAnimationData()[TrackIndex];

	for (int FrameIteration = 0; FrameIteration < FrameNumber; FrameIteration++)
	{
		const bool bShouldSetPosition = RawTrack.PosKeys.IsValidIndex(FrameIteration);
		const bool bShouldSetRotation = RawTrack.RotKeys.IsValidIndex(FrameIteration);
		const bool bShouldSetScale = RawTrack.ScaleKeys.IsValidIndex(FrameIteration);

		FTransform Mirror_Transform;

		if (bShouldSetPosition) Mirror_Transform.SetTranslation(RawTrack.PosKeys[FrameIteration]);
		if (bShouldSetRotation) Mirror_Transform.SetRotation(RawTrack.RotKeys[FrameIteration]);
		if (bShouldSetScale) Mirror_Transform.SetScale3D(RawTrack.ScaleKeys[FrameIteration]);

		Mirror_Transform.Mirror(TrackRules.MirrorAxis, TrackRules.FlipAxis);
		//Mirror_Transform.SetRotation(Mirror_Transform.GetRotation() + TrackRules.RotationDifference.Quaternion());
		/* TALVEZ REMOVER NORMALIZACAO? */
		//Mirror_Transform.NormalizeRotation();
		Mirror_Transform.SetScale3D(Mirror_Transform.GetScale3D().GetAbs());

		if (bShouldSetPosition) MirroredPositionKeys.Add(Mirror_Transform.GetTranslation());
		if (bShouldSetRotation) MirroredRotationKeys.Add(Mirror_Transform.GetRotation());
		if (bShouldSetScale) MirroredScaleKeys.Add(Mirror_Transform.GetScale3D());
	}

	Mirrored.PosKeys = MirroredPositionKeys;
	Mirrored.RotKeys = MirroredRotationKeys;
	Mirrored.ScaleKeys = MirroredScaleKeys;

	return true;
}

void MirrorHelpers::ProcessMirrorTrack(UAnimSequence* SequenceToMirror, const UMirrorPoseData* DataPose)
{
	if (!SequenceToMirror || SequenceToMirror->GetSkeleton() != DataPose->Skeleton || !DataPose) return;
	const FReferenceSkeleton& Skeleton = SequenceToMirror->GetSkeleton()->GetReferenceSkeleton();

	for (FSingleBoneMirror SingleBoneItem : DataPose->SingleBones)
	{
		FRawAnimSequenceTrack Mirrored;
		SingleBoneMirror(SequenceToMirror, SingleBoneItem, Mirrored);
		SequenceToMirror->AddNewRawTrack(FName(*SingleBoneItem.BoneName), &Mirrored);
	}

	for (FDoubleBoneMirror DoubleBoneItem : DataPose->DoubleBones)
	{		
		FRawAnimSequenceTrack Left_Mirrored;
		FRawAnimSequenceTrack Right_Mirrored;
		DoubleBoneMirror(SequenceToMirror, DoubleBoneItem, Left_Mirrored, Right_Mirrored);
		SequenceToMirror->AddNewRawTrack(FName(*DoubleBoneItem.RBoneName), &Left_Mirrored);
		SequenceToMirror->AddNewRawTrack(FName(*DoubleBoneItem.LBoneName), &Right_Mirrored);
	}

	//SequenceToMirror->PostProcessSequence();
	SequenceToMirror->ClearBakedTransformData();
	SequenceToMirror->RawCurveData.TransformCurves.Empty();
	SequenceToMirror->bNeedsRebake = false;
	SequenceToMirror->MarkRawDataAsModified();
	SequenceToMirror->OnRawDataChanged();
	SequenceToMirror->MarkPackageDirty();
		
		/*TArray<FName> TrackNames = AnimSequence->GetAnimationTrackNames();

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
	*/
		
}