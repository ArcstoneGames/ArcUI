// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUIFrameworkEditor.h"
// ArcUIFrameworkEditor
#include "SArcUIDebugWidget.h"
// ArcUIFramework
#include "ArcUITester.h"
#include "ArcUITesterCustomization.h"
// UE
#include "Framework/Docking/TabManager.h"
#include "Internationalization/Internationalization.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FArcUIFrameworkEditorModule"

namespace ArcUIEditor
{
	static const FName DebugTabId{TEXT("ArcUIDebug")};
}

void FArcUIFrameworkEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(AArcUITester::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FArcUITesterCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(FArcUIViewPayloadWrapper::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FArcUIViewPayloadWrapperCustomization::MakeInstance));

	// Register the dockable tab spawner
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		ArcUIEditor::DebugTabId,
		FOnSpawnTab::CreateLambda([](const FSpawnTabArgs&)
		{
			return SNew(SDockTab)
				.TabRole(ETabRole::NomadTab)
				[
					SNew(SArcUIDebugWidget)
				];
		}))
		.SetDisplayName(LOCTEXT("ArcUIDebugTab", "ArcUI Debug"))
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "DeveloperTools.MenuIcon"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	// Add menu entry to the shared Arcstone Games menu if present, otherwise create a standalone Arcstone UI menu.
	// The fallback menu is registered by us, so IsMenuRegistered is reliable for it.
	// For the ArcCore menu we use ExtendMenu directly — it merges in when ArcCore registers it.
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
	{
		UToolMenus* ToolMenus = UToolMenus::Get();
		const FName ArcstoneMenuName  = TEXT("MainFrame.MainMenu.ArcstoneMenu");
		const FName FallbackMenuName  = TEXT("MainFrame.MainMenu.ArcstoneUIMenu");

		const bool bHasArcCore = FModuleManager::Get().IsModuleLoaded(TEXT("ArcCoreEditor"));
		const FName TargetMenuName = bHasArcCore ? ArcstoneMenuName : FallbackMenuName;

		if (!bHasArcCore && !ToolMenus->IsMenuRegistered(FallbackMenuName))
		{
			ToolMenus->RegisterMenu(FallbackMenuName);
			ToolMenus->ExtendMenu(TEXT("MainFrame.MainMenu"))
				->FindOrAddSection(TEXT("ArcstoneUIMenuBar"))
				.AddSubMenu(
					TEXT("ArcstoneUIMenu"),
					LOCTEXT("ArcstoneUIMenu_Label", "Arcstone UI"),
					LOCTEXT("ArcstoneUIMenu_Tooltip", "Arcstone UI tools and utilities"),
					FNewToolMenuDelegate()
				);
		}

		auto* Menu = ToolMenus->ExtendMenu(TargetMenuName);
		auto& Section = Menu->FindOrAddSection(TEXT("ArcstoneUI"), LOCTEXT("ArcstoneUISection", "Arcstone UI"));
		Section.AddMenuEntry(
			TEXT("OpenArcUIDebug"),
			LOCTEXT("ArcUIDebugMenu_Label", "ArcUI Debug"),
			LOCTEXT("ArcUIDebugMenu_Tooltip", "Open the ArcUI runtime debug panel"),
			FSlateIcon(FName("CoreStyle"), "Icons.Toolbar.Debug"),
			FUIAction(FExecuteAction::CreateLambda([]()
			{
				FGlobalTabmanager::Get()->TryInvokeTab(ArcUIEditor::DebugTabId);
			}))
		);
	}));
}

void FArcUIFrameworkEditorModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout(AArcUITester::StaticClass()->GetFName());
	PropertyModule.UnregisterCustomPropertyTypeLayout(FArcUIViewPayloadWrapper::StaticStruct()->GetFName());

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ArcUIEditor::DebugTabId);
	UToolMenus::UnregisterOwner(this);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FArcUIFrameworkEditorModule, ArcUIFrameworkEditor)
