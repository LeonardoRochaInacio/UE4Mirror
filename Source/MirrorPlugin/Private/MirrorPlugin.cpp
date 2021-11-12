#include "MirrorPlugin.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "FMirrorPluginModule"

DEFINE_LOG_CATEGORY(LogMirrorPlugin);

void FMirrorPluginModule::StartupModule()
{
	
}

void FMirrorPluginModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMirrorPluginModule, MirrorPlugin)