// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlaneViewport.h"

void SPlaneViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());

	PreviewComponent = NewObject<UDebugSkelMeshComponent>();
	PreviewComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	PreviewScene.AddComponent(PreviewComponent, FTransform::Identity);

	SetSkeleton(InArgs._Skeleton);
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
				//				PreviewComponent->AnimScriptInstance = PreviewComponent->PreviewInstance;
				PreviewComponent->PreviewInstance->SetForceRetargetBasePose(true);
				PreviewComponent->RefreshBoneTransforms(nullptr);


				FVector NewPosition = Client->GetViewLocation();
				NewPosition.Normalize();
				float ImportBoundsSphereRadius = PreviewSkeletalMesh->GetImportedBounds().SphereRadius;
				NewPosition *= (ImportBoundsSphereRadius * 1.2f);
				NewPosition += FVector(0.0f, 0.0f, ImportBoundsSphereRadius * 0.25f);
				Client->SetViewLocation(NewPosition);
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
	TSharedPtr<FEditorViewportClient> EditorViewportClient = MakeShareable(new FPlaneViewportClient(PreviewScene, SharedThis(this)));

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