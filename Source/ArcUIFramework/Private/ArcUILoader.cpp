// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUILoader.h"

// ArcUI
#include "ArcUILayout.h"
#include "ArcUILog.h"
#include "ArcUIPresenter.h"
#include "ArcUISettings.h"
#include "ArcUISubsystem.h"
#include "ArcUITags.h"
#include "ArcUIViewInfo.h"
// UE5
#include "DataRegistrySubsystem.h"
#include "Blueprint/UserWidget.h"
// generated

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcUILoader)

namespace ArcUILoader
{
	void ForEachRegisteredViewInfo(const TFunction<void(const FArcUIViewInfo&)>& Task)
	{
		const auto* ArcUISettings = GetDefault<UArcUISettings>();

		for (const auto& ViewInfo : ArcUISettings->Views)
		{
			Task(ViewInfo);
		}

		if (const auto* Subsystem = UDataRegistrySubsystem::Get())
		{
			if (const auto* Registry = Subsystem->GetRegistryForType(ArcUISettings->ViewsRegistryName))
			{
				constexpr bool bSortForDisplay = false;
				TArray<FDataRegistryId> RegistryIds;
				Registry->GetPossibleRegistryIds(RegistryIds, bSortForDisplay);
				for (auto ItemId : RegistryIds)
				{
					if (auto* ViewInfo = Registry->GetCachedItem<FArcUIViewInfo>(ItemId))
					{
						Task(*ViewInfo);
					}
				}
			}
		}
	}

	void ForEachRegisteredPresenter(const TFunction<void(const FArcUIPresenterInfo&)>& Task)
	{
		const auto* ArcUISettings = GetDefault<UArcUISettings>();

		for (const auto& PresenterInfo : ArcUISettings->Presenters)
		{
			Task(PresenterInfo);
		}

		if (const auto* Subsystem = UDataRegistrySubsystem::Get())
		{
			if (const auto* Registry = Subsystem->GetRegistryForType(ArcUISettings->PresentersRegistryName))
			{
				constexpr bool bSortForDisplay = false;
				TArray<FDataRegistryId> RegistryIds;
				Registry->GetPossibleRegistryIds(RegistryIds, bSortForDisplay);
				for (auto ItemId : RegistryIds)
				{
					if (auto* PresenterInfo = Registry->GetCachedItem<FArcUIPresenterInfo>(ItemId))
					{
						Task(*PresenterInfo);
					}
				}
			}
		}
	}
}

void UArcUILoader::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UArcUISettings* ArcUISettings = GetDefault<UArcUISettings>();

	// load the layout, if any
	if (!ArcUISettings->LayoutClass.IsNull())
	{
		LayoutClass = ArcUISettings->LayoutClass.LoadSynchronous();
		ensure(LayoutClass);
	}

	// for the default loading of presenter we need to ensure the UISubsystem is ready, as we'll register the newly created presenters
	// but the UArcUISubsystem will leave its init to its child class so we have to fetch it
	TArray<UClass*> ChildClasses;
	GetDerivedClasses(UArcUISubsystem::StaticClass(), ChildClasses, false);
	if (ChildClasses.Num() > 0)
	{
		ensureMsgf(ChildClasses.Num() == 1, TEXT("More than one derived class of UISubsystem - This may not be properly handled"));
		Collection.InitializeDependency(ChildClasses[0]);
	}
	else
	{
		Collection.InitializeDependency<UArcUISubsystem>();
	}

	// default loading of some presenters meant to be always loaded
	ArcUILoader::ForEachRegisteredPresenter([this](const FArcUIPresenterInfo& PresenterInfo)
	{
		if (PresenterInfo.bKeepAlwaysLoaded)
		{
			// Using the global Tag here as these Presenters are not tied to any context
			CreatePresenter(ArcUITags::Context, PresenterInfo);
		}
	});
	
	// default loading of some views meant to be always loaded
	ArcUILoader::ForEachRegisteredViewInfo([this](const FArcUIViewInfo& ViewInfo)
	{
		if (ViewInfo.bKeepAlwaysLoaded)
		{
			LoadWidgetSubclass(ViewInfo.ViewTag, ViewInfo.WidgetClass);
		}
	});
}

bool UArcUILoader::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined
		return ChildClasses.Num() == 0;
	}

	return false;
}

void UArcUILoader::OnContextAdded(FGameplayTag ContextTag)
{
	auto* UISubsystem = GetGameInstance()->GetSubsystem<UArcUISubsystem>();

	ArcUILoader::ForEachRegisteredPresenter([this, &UISubsystem, &ContextTag](const FArcUIPresenterInfo& PresenterInfo)
	{
		if (PresenterInfo.bLoadingTiedToContext && PresenterInfo.ContextTag == ContextTag)
		{
			CreatePresenter(ContextTag, PresenterInfo);
		}
	});

	ArcUILoader::ForEachRegisteredViewInfo([this, &UISubsystem, &ContextTag](const FArcUIViewInfo& ViewInfo)
	{
		if (const auto* ContextAction = ViewInfo.ContextActions.FindByPredicate([&ContextTag](const auto& Action)
			{ return ContextTag == Action.ContextTag; }))
		{
			// (pre) loading
			if (ContextAction->bLoadingTiedToContext)
			{
				LoadWidgetSubclass(ViewInfo.ViewTag, ViewInfo.WidgetClass);
			}

			// creation
			if (ContextAction->bCreationTiedToContext)
			{
				UISubsystem->CreateWidgetOnLayout(ViewInfo.ViewTag, ContextAction->ContextTag, ContextAction->CreateOnLayerTag, ContextAction->SlotName);
			}
		}
	});
}

void UArcUILoader::OnContextRemoved(FGameplayTag ContextTag)
{
	auto* UISubsystem = GetGameInstance()->GetSubsystem<UArcUISubsystem>();

	ArcUILoader::ForEachRegisteredPresenter(
	[&ContextTag, &UISubsystem, &ManagedClasses=ManagedPresenterClasses, &ManagedPresenters=ManagedPresenters](const FArcUIPresenterInfo& PresenterInfo)
	{
		if (!PresenterInfo.bKeepAlwaysLoaded && PresenterInfo.bLoadingTiedToContext && PresenterInfo.ContextTag == ContextTag)
		{
			ManagedClasses.Remove(ContextTag);

			if (auto* Found = ManagedPresenters.Find(ContextTag))
			{
				// /!\ this condition may not be enough!
				if (const int32 Index = Found->Presenters.IndexOfByPredicate([ContextTag](const UArcUIPresenter* Presenter) { return Presenter->HasContextTag(ContextTag); });
					Index != INDEX_NONE)
				{
					UISubsystem->UnRegisterPresenter(Found->Presenters[Index]);
					Found->Presenters.RemoveAt(Index);
				}

				if (Found->Presenters.IsEmpty())
				{
					ManagedPresenters.Remove(ContextTag);
				}
			}
			ManagedPresenters.Remove(ContextTag);
		}
	});

	ArcUILoader::ForEachRegisteredViewInfo([&UISubsystem, &ContextTag, &ManagedClasses=ManagedWidgetClasses](const FArcUIViewInfo& ViewInfo)
	{
		if (const auto* ContextAction = ViewInfo.ContextActions.FindByPredicate(
			[&ContextTag](const auto& Action) { return ContextTag == Action.ContextTag; }))
		{
			if (!ViewInfo.bKeepAlwaysLoaded && ContextAction->bLoadingTiedToContext)
			{
				ManagedClasses.Remove(ContextTag);
			}

			if (ContextAction->bCreationTiedToContext)
			{
				UISubsystem->DestroyWidget(ViewInfo.ViewTag, ContextAction->ContextTag, ContextAction->CreateOnLayerTag);
			}
		}
	});
}

UClass* UArcUILoader::GetWidgetClass_Internal(FGameplayTag ViewTag, FGameplayTag ContextTag)
{
	if (const auto& Subclass = GetLoadedWidgetSubclass(ViewTag))
	{
		return Subclass.Get();
	}

	UClass* WidgetClass {nullptr};
	ArcUILoader::ForEachRegisteredViewInfo([this, &ViewTag, &WidgetClass](const FArcUIViewInfo& ViewInfo)
	{
		if (ViewInfo.ViewTag == ViewTag)
		{
			WidgetClass = LoadWidgetSubclass(ViewTag, ViewInfo.WidgetClass).Get();
		}
	});
	
	if (WidgetClass)
	{
		return WidgetClass;
	}

	UE_LOG(LogArcUI, Error,
		TEXT("GetWidgetClass_Internal - Could not load widget from view [%s] in context [%s]"),
		*ViewTag.ToString(), *ContextTag.ToString());
	return nullptr;
}

void UArcUILoader::CreatePresenter(FGameplayTag ContextTag, const FArcUIPresenterInfo& PresenterInfo)
{
	auto* UISubsystem = GetGameInstance()->GetSubsystem<UArcUISubsystem>();

	const auto PresenterClass = LoadPresenterSubclass(PresenterInfo.ContextTag, PresenterInfo.Presenter);

	auto* Presenter = NewObject<UArcUIPresenter>(this, PresenterClass.Get());
	ManagedPresenters.FindOrAdd(ContextTag).Presenters.Add(Presenter);
	UISubsystem->RegisterPresenter(Presenter);
}

TSubclassOf<UArcUIPresenter> UArcUILoader::LoadPresenterSubclass(FGameplayTag ViewTag, const TSoftClassPtr<UArcUIPresenter>& AssetPointer)
{
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	const auto& Subclass = Cast<UClass>(AssetPath.TryLoad());
	ManagedPresenterClasses.Add(ViewTag, Subclass);
	return Subclass;
}

TSubclassOf<UArcUIPresenter> UArcUILoader::GetLoadedPresenterSubclass(FGameplayTag ViewTag)
{
	if (const auto* Found = ManagedPresenterClasses.Find(ViewTag))
	{
		return *Found;
	}
	return nullptr;
}

TSubclassOf<UUserWidget> UArcUILoader::LoadWidgetSubclass(FGameplayTag ViewTag, const TSoftClassPtr<UUserWidget>& AssetPointer)
{
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	const auto& Subclass = Cast<UClass>(AssetPath.TryLoad());
	ManagedWidgetClasses.Add(ViewTag, Subclass);
	return Subclass;
}

TSubclassOf<UUserWidget> UArcUILoader::GetLoadedWidgetSubclass(FGameplayTag ViewTag)
{
	if (const auto* Found = ManagedWidgetClasses.Find(ViewTag))
	{
		return *Found;
	}
	return nullptr;
}
