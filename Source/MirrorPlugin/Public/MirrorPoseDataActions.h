// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "AssetTypeActions_Base.h"

class FMirrorPoseDataActions : public FAssetTypeActions_Base
{
public:
	
    //FMirrorPoseDataActions(EAssetTypeCategories::Type InAssetCategory);

    virtual FText GetName() const override;
    virtual FColor GetTypeColor() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual uint32 GetCategories() override;
};
