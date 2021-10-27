// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SEditorViewport.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "AnimPreviewInstance.h"
#include "EditorViewportClient.h"
#include "Settings/SkeletalMeshEditorSettings.h"

/**
 * 
 */
class MIRRORPLUGIN_API SPlaneViewport : public /*SCompoundWidget*/SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SPlaneViewport)
	{}

	SLATE_ARGUMENT(USkeleton*, Skeleton)
	SLATE_END_ARGS()

public:
	
	SPlaneViewport();

	void Construct(const FArguments& InArgs);
	void SetSkeleton(USkeleton* Skeleton);

protected:
	/** SEditorViewport interface */
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;

private:
	
	/** Skeleton */
	USkeleton* TargetSkeleton;

	FPreviewScene PreviewScene;

	class UDebugSkelMeshComponent* PreviewComponent;

	bool IsVisible() const override;
};


class FPlaneViewportClient : public FEditorViewportClient
{
public:
	FPlaneViewportClient(FPreviewScene& InPreviewScene, const TSharedRef<SPlaneViewport>& InBasePoseViewport)
		: FEditorViewportClient(nullptr, &InPreviewScene, StaticCastSharedRef<SEditorViewport>(InBasePoseViewport))
	{
		SetViewMode(VMI_Lit);

		// Always composite editor objects after post processing in the editor
		EngineShowFlags.SetCompositeEditorPrimitives(true);
		EngineShowFlags.DisableAdvancedFeatures();

		UpdateLighting();

		// Setup defaults for the common draw helper.
		DrawHelper.bDrawPivot = false;
		DrawHelper.bDrawWorldBox = false;
		DrawHelper.bDrawKillZ = false;
		DrawHelper.bDrawGrid = true;
		DrawHelper.GridColorAxis = FColor(70, 70, 70);
		DrawHelper.GridColorMajor = FColor(0, 0, 0);
		DrawHelper.GridColorMinor = FColor(0, 0, 0);
		DrawHelper.PerspectiveGridSize = HALF_WORLD_MAX1;
		DrawHelper.AxesLineThickness = 3.0f;
		bDisableInput = true;

		

	}


	// FEditorViewportClient interface
	virtual void Tick(float DeltaTime) override
	{
		if (PreviewScene)
		{
			PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaTime);
		}
	}

	virtual FSceneInterface* GetScene() const override
	{
		return PreviewScene->GetScene();
	}

	virtual FLinearColor GetBackgroundColor() const override
	{
		return FLinearColor::White;
	}

	// End of FEditorViewportClient

	void UpdateLighting()
	{
		const USkeletalMeshEditorSettings* Options = GetDefault<USkeletalMeshEditorSettings>();

		PreviewScene->SetLightDirection(Options->AnimPreviewLightingDirection);
		PreviewScene->SetLightColor(Options->AnimPreviewDirectionalColor);
		PreviewScene->SetLightBrightness(Options->AnimPreviewLightBrightness);
		//GetScene()->AddPrimitive();
	}
};