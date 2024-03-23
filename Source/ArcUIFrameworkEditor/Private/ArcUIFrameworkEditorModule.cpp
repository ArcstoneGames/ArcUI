﻿// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUIFrameworkEditor.h"

#include "ArcUITester.h"
#include "ArcUITesterCustomization.h"

#define LOCTEXT_NAMESPACE "FArcUIFrameworkEditorModule"

void FArcUIFrameworkEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(AArcUITester::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FArcUITesterCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("ArcUITester_ModelWrapper", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FArcUIModelWrapperCustomization::MakeInstance));
}

void FArcUIFrameworkEditorModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout(AArcUITester::StaticClass()->GetFName());
	PropertyModule.UnregisterCustomPropertyTypeLayout("ArcUITester_ModelWrapper");
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FArcUIFrameworkEditorModule, ArcUIFrameworkEditor)