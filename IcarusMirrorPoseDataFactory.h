
#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "IcarusMirrorPoseDataFactory.generated.h"

UCLASS()
class ICARUSEDITOR_API UIcarusMirrorPoseDataFactory : public UFactory
{
	GENERATED_BODY()

public:

	UIcarusMirrorPoseDataFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual uint32 GetMenuCategories() const override;
};