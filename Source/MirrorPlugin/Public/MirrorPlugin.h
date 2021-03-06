#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMirrorPlugin, Log, All);

class FMirrorPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FSlateStyleSet> Style;
};
