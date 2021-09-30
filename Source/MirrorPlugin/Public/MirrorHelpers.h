// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MirrorPoseData.h"

/**
 * 
 */
class MIRRORPLUGIN_API MirrorHelpers
{
private:

	MirrorHelpers() {};
	~MirrorHelpers() {};

public:

	static void SingleBoneMirror(FSingleBoneMirror BoneRules ,FVector BasePoseLocation, FRotator BasePoseRotation, FVector FramePoseLocation, FRotator FramePoseRotation, FVector& LocationResult, FRotator& RotationResult);
	static void DoubleBoneRotationMirror(FDoubleBoneMirror BoneRules, FRotator L_BasePoseRotation, FRotator R_BasePoseRotation, FRotator L_FramePoseRotation, FRotator R_FramePoseRotation, FRotator & L_RotationResult, FRotator & R_RotationResult);
	static void SingleTrackMirror(FRawAnimSequenceTrack& Track, FTransform BaseBoneTransform, FSingleBoneMirror TrackRules);
	static void DoubleTrackMirror(FRawAnimSequenceTrack& L_Track, FTransform L_BaseBoneTransform, FRawAnimSequenceTrack& R_Track, FTransform R_BaseBoneTransform, FDoubleBoneMirror TrackRules);
};
