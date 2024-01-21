// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUILoader.h"

// ArcUI
#include "ArcUILog.h"
#include "ArcUILayout.h"
#include "ArcUISubsystem.h"
#include "ArcUISettings.h"
// UE5
#include "DataRegistrySubsystem.h"
#include "Blueprint/UserWidget.h"
// generated
#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcUILoader)

namespace ArcUILoader
{
	void ForEachRegisteredAsset(const TFunction<void(const FArcUIAsset&)>& Task)
	{
		const UArcUISettings* ArcUISettings = GetDefault<UArcUISettings>();
		if (const auto* Subsystem = UDataRegistrySubsystem::Get())
		{
			if (const auto* Registry = Subsystem->GetRegistryForType(ArcUISettings->RegistryName))
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
}

void UArcUILoader::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UArcUISubsystem>();

	const UArcUISettings* ArcUISettings = GetDefault<UArcUISettings>();

	// load the layout, if any
	if (!ArcUISettings->LayoutClass.IsNull())
	{
		LayoutClass = ArcUISettings->LayoutClass.LoadSynchronous();
		ensure(LayoutClass);
	}
	
	// default loading of some assets meant to be always loaded
	ArcUILoader::ForEachRegisteredAsset([this](const FArcUIAsset& UIAsset)
	{
		if (UIAsset.bKeepAlwaysLoaded)
		{
			LoadSubclass(UIAsset.AssetTag, UIAsset.WidgetClass);
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
	UArcUISubsystem* UISubsystem = GetGameInstance()->GetSubsystem<UArcUISubsystem>();
	ArcUILoader::ForEachRegisteredAsset([this, &UISubsystem, &ContextTag](const FArcUIAsset& UIAsset)
	{
		if (const auto* ContextAction = UIAsset.ContextActions.FindByPredicate([&ContextTag](const auto& Action)
			{ return ContextTag == Action.ContextTag; }))
		{
			// (pre) loading
			if (ContextAction->bLoadingTiedToContext)
			{
				LoadSubclass(UIAsset.AssetTag, UIAsset.WidgetClass);
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
	UArcUISubsystem* UISubsystem = GetGameInstance()->GetSubsystem<UArcUISubsystem>();
	ArcUILoader::ForEachRegisteredAsset([&UISubsystem, &ContextTag](const FArcUIAsset& UIAsset)
	{
		if (const auto* ContextAction = UIAsset.ContextActions.FindByPredicate(
			[&ContextTag](const auto& Action) { return ContextTag == Action.ContextTag; }))
		{
			if (ContextAction->bLoadingTiedToContext && !UIAsset.bKeepAlwaysLoaded)
			{
				// Unload subclass?
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
	if (const auto& Subclass = GetLoadedSubclass(AssetTag))
	{
		return Subclass.Get();
	}

	UClass* AssetClass {nullptr};
	ArcUILoader::ForEachRegisteredAsset([this, &AssetTag, &AssetClass](const FArcUIAsset& UIAsset)
	{
		if (UIAsset.AssetTag == AssetTag)
		{
			AssetClass = LoadSubclass(AssetTag, UIAsset.WidgetClass).Get();
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

TSubclassOf<UUserWidget> UArcUILoader::LoadSubclass(FGameplayTag AssetTag, const TSoftClassPtr<UUserWidget>& AssetPointer)
{
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	const auto& Subclass = Cast<UClass>(AssetPath.TryLoad());
	ManagedClasses.Add(AssetTag, Subclass);
	return Subclass;
}

TSubclassOf<UUserWidget> UArcUILoader::GetLoadedSubclass(FGameplayTag AssetTag)
{
	if (const auto* Found = ManagedClasses.Find(AssetTag))
	{
		return *Found;
	}
	return nullptr;
}
