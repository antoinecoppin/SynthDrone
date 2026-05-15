// Copyright Plasma Labs, 2023. All Rights Reserved.

#include "BoundingBox2D.h"

#define LOCTEXT_NAMESPACE "FBoundingBox2DModule"

void FBoundingBox2DModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FBoundingBox2DModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBoundingBox2DModule, BoundingBox2D)