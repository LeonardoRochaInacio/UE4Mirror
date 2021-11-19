// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Animation/Skeleton.h"
#include "MirrorPoseData.generated.h"

USTRUCT(BlueprintType)
struct MIRRORPLUGIN_API FSingleBoneMirror
{
	GENERATED_USTRUCT_BODY()

	FSingleBoneMirror() {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool RotationX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool RotationY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool RotationZ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool LocationX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool LocationY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool LocationZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAxis::Type> MirrorAxis = EAxis::Type::X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAxis::Type> FlipAxis = EAxis::Type::X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationDifference = FRotator(0.0f);
};

USTRUCT(BlueprintType)
struct MIRRORPLUGIN_API FDoubleBoneMirror
{
	GENERATED_USTRUCT_BODY()

	FDoubleBoneMirror() {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString LBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString RBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool RotationX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool RotationY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool RotationZ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool LocationX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool LocationY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool LocationZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAxis::Type> MirrorAxis = EAxis::Type::X;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAxis::Type> FlipAxis = EAxis::Type::X;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationDifference = FRotator(0.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldMirrorTranslation = true;
};


UCLASS(BlueprintType)
class MIRRORPLUGIN_API UMirrorPoseData final : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SkeletonSettings)
	USkeleton* Skeleton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GeneralSettings)
	TArray<FSingleBoneMirror> SingleBones;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GeneralSettings)
	TArray<FDoubleBoneMirror> DoubleBones;

};