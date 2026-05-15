// Copyright Plasma Labs, 2023. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FBoundingBox2DModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
