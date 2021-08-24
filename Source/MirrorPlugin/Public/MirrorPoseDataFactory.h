
#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MirrorPoseDataFactory.generated.h"

UCLASS()
class EDITOR_API UMirrorPoseDataFactory : public UFactory
{
	GENERATED_BODY()

public:

	UMirrorPoseDataFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual uint32 GetMenuCategories() const override;
};