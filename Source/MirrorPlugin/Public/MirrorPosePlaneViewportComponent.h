// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "MirrorPosePlaneViewportComponent.generated.h"

/**
 * 
 */
UCLASS()
class MIRRORPLUGIN_API UMirrorPosePlaneViewportComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

	UMirrorPosePlaneViewportComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
