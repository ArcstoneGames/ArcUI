// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FArcUIFrameworkEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
