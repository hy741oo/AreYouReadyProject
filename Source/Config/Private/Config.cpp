// Copyright U.N.Owen, All Rights Reserved.

#include "Config.h"

void FConfigModule::StartupModule()
{
	// This code will execute after you module is loaded into memory; the exact timing is specified in the .uplugin file per-module.
}

void FConfigModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

IMPLEMENT_MODULE(FConfigModule, Config)