// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUILoader.h"

// ArcUI
#include "ArcUILayout.h"
#include "ArcUILog.h"
#include "ArcUIPresenter.h"
#include "ArcUISettings.h"
#include "ArcUISubsystem.h"
#include "ArcUITags.h"
// UE5
#include "DataRegistrySubsystem.h"
#include "Blueprint/UserWidget.h"
// generated

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcUILoader)

namespace ArcUILoader
{
	void ForEachRegisteredAsset(const TFunction<void(const FArcUIAsset&)>& Task)
	{
		const auto* ArcUISettings = GetDefault<UArcUISettings>();

		for (const auto& Asset : ArcUISettings->Assets)
		{
			Task(Asset);
		}

		if (const auto* Subsystem = UDataRegistrySubsystem::Get())
		{
			if (const auto* Registry = Subsystem->GetRegistryForType(ArcUISettings->AssetsRegistryName))
			{
				constexpr bool bSortForDisplay = false;
				TArray<FDataRegistryId> RegistryIds;
				Registry->GetPossibleRegistryIds(RegistryIds, bSortForDisplay);
				for (auto ItemId : RegistryIds)
				{
					if (auto* ArcUIAsset = Registry->GetCachedItem<FArcUIAsset>(ItemId))
					{
						Task(*ArcUIAsset);
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
	
	// default loading of some assets meant to be always loaded
	ArcUILoader::ForEachRegisteredAsset([this](const FArcUIAsset& UIAsset)
	{
		if (UIAsset.bKeepAlwaysLoaded)
		{
			LoadWidgetSubclass(UIAsset.AssetTag, UIAsset.WidgetClass);
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

	ArcUILoader::ForEachRegisteredAsset([this, &UISubsystem, &ContextTag](const FArcUIAsset& UIAsset)
	{
		if (const auto* ContextAction = UIAsset.ContextActions.FindByPredicate([&ContextTag](const auto& Action)
			{ return ContextTag == Action.ContextTag; }))
		{
			// (pre) loading
			if (ContextAction->bLoadingTiedToContext)
			{
				LoadWidgetSubclass(UIAsset.AssetTag, UIAsset.WidgetClass);
			}

			// creation
			if (ContextAction->bCreationTiedToContext)
			{
				UISubsystem->CreateWidgetOnLayout(UIAsset.AssetTag, ContextAction->ContextTag, ContextAction->CreateOnLayerTag);
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

	ArcUILoader::ForEachRegisteredAsset([&UISubsystem, &ContextTag, &ManagedClasses=ManagedWidgetClasses](const FArcUIAsset& UIAsset)
	{
		if (const auto* ContextAction = UIAsset.ContextActions.FindByPredicate(
			[&ContextTag](const auto& Action) { return ContextTag == Action.ContextTag; }))
		{
			if (!UIAsset.bKeepAlwaysLoaded && ContextAction->bLoadingTiedToContext)
			{
				ManagedClasses.Remove(ContextTag);
			}

			if (ContextAction->bCreationTiedToContext)
			{
				UISubsystem->DestroyWidget(UIAsset.AssetTag, ContextAction->ContextTag, ContextAction->CreateOnLayerTag);
			}
		}
	});
}

UClass* UArcUILoader::GetAssetClass_Internal(FGameplayTag AssetTag, FGameplayTag ContextTag)
{
	if (const auto& Subclass = GetLoadedWidgetSubclass(AssetTag))
	{
		return Subclass.Get();
	}

	UClass* AssetClass {nullptr};
	ArcUILoader::ForEachRegisteredAsset([this, &AssetTag, &AssetClass](const FArcUIAsset& UIAsset)
	{
		if (UIAsset.AssetTag == AssetTag)
		{
			AssetClass = LoadWidgetSubclass(AssetTag, UIAsset.WidgetClass).Get();
		}
	});
	
	if (AssetClass)
	{
		return AssetClass;
	}

	UE_LOG(LogArcUI, Error,
		TEXT("GetAssetClass_Internal - Could not load asset from tag [%s] in context [%s]"),
		*AssetTag.ToString(), *ContextTag.ToString());
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

TSubclassOf<UArcUIPresenter> UArcUILoader::LoadPresenterSubclass(FGameplayTag AssetTag, const TSoftClassPtr<UArcUIPresenter>& AssetPointer)
{
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	const auto& Subclass = Cast<UClass>(AssetPath.TryLoad());
	ManagedPresenterClasses.Add(AssetTag, Subclass);
	return Subclass;
}

TSubclassOf<UArcUIPresenter> UArcUILoader::GetLoadedPresenterSubclass(FGameplayTag AssetTag)
{
	if (const auto* Found = ManagedPresenterClasses.Find(AssetTag))
	{
		return *Found;
	}
	return nullptr;
}

TSubclassOf<UUserWidget> UArcUILoader::LoadWidgetSubclass(FGameplayTag AssetTag, const TSoftClassPtr<UUserWidget>& AssetPointer)
{
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	const auto& Subclass = Cast<UClass>(AssetPath.TryLoad());
	ManagedWidgetClasses.Add(AssetTag, Subclass);
	return Subclass;
}

TSubclassOf<UUserWidget> UArcUILoader::GetLoadedWidgetSubclass(FGameplayTag AssetTag)
{
	if (const auto* Found = ManagedWidgetClasses.Find(AssetTag))
	{
		return *Found;
	}
	return nullptr;
}
