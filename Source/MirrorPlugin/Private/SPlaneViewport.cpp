// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlaneViewport.h"

void SPlaneViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());

	PreviewComponent = NewObject<UDebugSkelMeshComponent>();
	PreviewComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	PreviewScene.AddComponent(PreviewComponent, FTransform::Identity);
	EditorViewportClient->TargetSkeleton = PreviewComponent;
	SetSkeleton(InArgs._Skeleton);

	UMirrorPosePlaneViewportComponent* VisualMirrorPlane = NewObject<UMirrorPosePlaneViewportComponent>(GetWorld(), "VisualMirrorPlane");

	if (VisualMirrorPlane->IsValidLowLevel())
	{
		VisualMirrorPlane->SetWorldScale3D(FVector(100.0f));
		FTransform VisualMirrorPlaneTransform;
		VisualMirrorPlaneTransform.SetRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
		const float PreviewMeshSphereRadius = InArgs._Skeleton->GetPreviewMesh()->GetImportedBounds().SphereRadius;
		const float PlaneSize = 256.0f;
		const float PlaneScale = ((PreviewMeshSphereRadius * 2) / PlaneSize) + 2;
		VisualMirrorPlaneTransform.SetScale3D(FVector(PlaneScale));
		VisualMirrorPlaneTransform.SetLocation(FVector(0.0f, 0.0f, PlaneSize/4.0f));
		PreviewScene.AddComponent(VisualMirrorPlane, VisualMirrorPlaneTransform);
	}
}

void SPlaneViewport::SetSkeleton(USkeleton* Skeleton)
{
	if (Skeleton != TargetSkeleton)
	{
		TargetSkeleton = Skeleton;

		if (TargetSkeleton)
		{
			USkeletalMesh* PreviewSkeletalMesh = Skeleton->GetPreviewMesh();
			if (PreviewSkeletalMesh)
			{
				PreviewComponent->SetSkeletalMesh(PreviewSkeletalMesh);
				PreviewComponent->EnablePreview(true, nullptr);
				//PreviewComponent->AnimScriptInstance = PreviewComponent->PreviewInstance;
				PreviewComponent->PreviewInstance->SetForceRetargetBasePose(true);
				PreviewComponent->RefreshBoneTransforms(nullptr);
			}
			else
			{
				PreviewComponent->SetSkeletalMesh(nullptr);
			}
		}
		else
		{
			PreviewComponent->SetSkeletalMesh(nullptr);
		}

		Client->Invalidate();
	}
}

SPlaneViewport::SPlaneViewport()
	: PreviewScene(FPreviewScene::ConstructionValues())
{
}

bool SPlaneViewport::IsVisible() const
{
	return true;
}

TSharedRef<FEditorViewportClient> SPlaneViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(new FPlaneViewportClient(PreviewScene, SharedThis(this)));

	EditorViewportClient->ViewportType = LVT_Perspective;
	EditorViewportClient->bSetListenerPosition = false;
	EditorViewportClient->SetViewLocation(EditorViewportDefs::DefaultPerspectiveViewLocation);
	EditorViewportClient->SetViewRotation(EditorViewportDefs::DefaultPerspectiveViewRotation);
	EditorViewportClient->SetRealtime(false);
	EditorViewportClient->VisibilityDelegate.BindSP(this, &SPlaneViewport::IsVisible);
	EditorViewportClient->SetViewMode(VMI_Lit);

	return EditorViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SPlaneViewport::MakeViewportToolbar()
{
	return nullptr;
}