// Fill out your copyright notice in the Description page of Project Settings.


#include "MirrorPosePlaneViewportComponent.h"

#include "UObject/ConstructorHelpers.h"

UMirrorPosePlaneViewportComponent::UMirrorPosePlaneViewportComponent(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("StaticMesh'/MirrorPlugin/MirrorPlane.MirrorPlane'"));
	if(PlaneMesh.Succeeded())
	{
		SetStaticMesh(PlaneMesh.Object);
	}
}
