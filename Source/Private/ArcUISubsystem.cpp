// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUISubsystem.h"

// ArcUI
#include "ArcUILayout.h"
#include "ArcUILoader.h"
#include "ArcUILog.h"
#include "ArcUIPresenter.h"
// CommonUI
#include "CommonActivatableWidget.h"
// generated
#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcUISubsystem)

bool UArcUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UArcUISubsystem::AddContext(FGameplayTag ContextTag)
{
	if (!ensureMsgf(!ContextTags.HasTagExact(ContextTag), TEXT("AddContext - context already added: %s"), *ContextTag.ToString()))
	{
		return;
	}

	ContextTags.AddTag(ContextTag);
	UE_LOG(LogArcUI, Verbose, TEXT("AddContext: %s"), *ContextTag.ToString());
	
	GetGameInstance()->GetSubsystem<UArcUILoader>()->OnContextAdded(ContextTag);

	for (const auto& Presenter : Presenters)
	{
		if (ensureMsgf(Presenter, TEXT("ArcUISubsystem: stale registered presenter, make sure to call UnRegisterPresenter")) &&
			Presenter->GetContextTags().HasTag(ContextTag))
		{
			Presenter->OnContextAdded(ContextTag);
		}
	}
}

void UArcUISubsystem::AddExclusiveContext(FGameplayTag ContextTag)
{
	while (!ContextTags.IsEmpty())
	{
		RemoveContext(ContextTags.First());
	}

	ContextTags.Reset();

	AddContext(ContextTag);
}

void UArcUISubsystem::RemoveContext(FGameplayTag ContextTag)
{
	if (!ensureMsgf(ContextTags.HasTagExact(ContextTag), TEXT("RemoveContext - context already removed or never added: %s"), *ContextTag.ToString()))
	{
		return;
	}

	ContextTags.RemoveTag(ContextTag);
	UE_LOG(LogArcUI, Verbose, TEXT("RemoveContext: %s"), *ContextTag.ToString());
	
	for (const auto& Presenter : Presenters)
	{
		if (ensureMsgf(Presenter, TEXT("ArcUISubsystem: stale registered presenter, make sure to call UnRegisterPresenter")) &&
			Presenter->GetContextTags().HasTag(ContextTag))
		{
			Presenter->OnContextRemoved(ContextTag);
		}
	}
	
	GetGameInstance()->GetSubsystem<UArcUILoader>()->OnContextRemoved(ContextTag);
}

bool UArcUISubsystem::HasContext(FGameplayTag ContextTag) const
{
	return ContextTags.HasTagExact(ContextTag);
}

void UArcUISubsystem::BackupContext()
{
	UE_CLOG(!ContextTagsBackup.IsEmpty(), LogArcUI, Warning, TEXT("Backup Context requested while backup is not empty"));

	ContextTagsBackup = ContextTags;
}

void UArcUISubsystem::RestoreContext()
{
	UE_CLOG(ContextTagsBackup.IsEmpty(), LogArcUI, Warning, TEXT("Restore Context requested while backup is empty"));

	for (const auto& ContextTag : ContextTagsBackup)
	{
		AddContext(ContextTag);
	}

	ContextTagsBackup.Reset();
}

void UArcUISubsystem::RegisterPresenter(UArcUIPresenter* Presenter)
{
	if (Presenters.Contains(Presenter))
	{
		UE_LOG(LogArcUI, Warning, TEXT("RegisterPresenter - presenter %s already registered by %s"), *Presenter->GetName(), *Presenter->GetOuter()->GetName());
		return;
	}
	
	Presenters.Add(Presenter);
	UE_LOG(LogArcUI, Verbose, TEXT("RegisterPresenter - %s from %s"), *Presenter->GetName(), *Presenter->GetOuter()->GetName());

	Presenter->SetUISubsystem(this);

	for (const auto& ContextTag : ContextTags)
	{
		if (Presenter->GetContextTags().HasTag(ContextTag))
		{
			Presenter->OnContextAdded(ContextTag);
		}
	}
}

void UArcUISubsystem::UnRegisterPresenter(UArcUIPresenter* Presenter)
{
	UE_CLOG(!Presenters.Contains(Presenter), LogArcUI, Warning, TEXT("RegisterPresenter - presenter already unregistered (or never registered)"));
	
	Presenters.Remove(Presenter);
}

void UArcUISubsystem::OnPlayerAdded(ULocalPlayer* LocalPlayer)
{
	if (!Layout)
	{
		if (auto* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
		{
			CreateLayout(LocalPlayer, PlayerController);
		}
	}
}

void UArcUISubsystem::OnPlayerRemoved(ULocalPlayer* LocalPlayer)
{
	if (Layout && Layout->GetPlayerContext().GetLocalPlayer() == LocalPlayer)
	{
		DestroyLayout();
	}
}

UUserWidget* UArcUISubsystem::CreateWidgetOnLayout(FGameplayTag InAssetTag, FGameplayTag InContextTag, FGameplayTag InLayerTag)
{
	if (!Layout)
	{
		UE_LOG(LogArcUI, Warning, TEXT("CreateWidgetOnLayout - No layout to create widget on, from Asset [%s], with context [%s] on layer [%s]"),
			*InAssetTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
		return nullptr;
	}

	const TSubclassOf<UUserWidget> WidgetClass = GetGameInstance()->GetSubsystem<UArcUILoader>()->GetAssetClass<UUserWidget>(InAssetTag, InContextTag);
	if (WidgetClass.Get() == nullptr)
	{
		UE_LOG(LogArcUI, Warning, TEXT("CreateWidgetOnLayout - Could not create widget from Asset [%s], with context [%s] on layer [%s]"),
			*InAssetTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
		return nullptr;
	}
	
	if (const TSubclassOf<UCommonActivatableWidget> ActivatableClass{WidgetClass})
	{
		auto* NewWidget = Layout->PushWidgetToLayer(InLayerTag, ActivatableClass);
		ManagedWidgets.Add({NewWidget, InAssetTag, InContextTag, InLayerTag});
		UE_LOG(LogArcUI, Verbose, TEXT("CreateWidgetOnLayout - Widget %s from Asset [%s], with context [%s] was pushed on layer [%s]"),
			*NewWidget->GetName(), *InAssetTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
		return NewWidget;
	}

	UUserWidget* NewWidget{nullptr};
	if (auto* ParentWidget = Layout->GetActiveWidgetOnLayer(InLayerTag))
	{
		NewWidget = CreateWidget<UUserWidget>(ParentWidget, WidgetClass);
		NewWidget->AddToPlayerScreen();
		ManagedWidgets.Add({NewWidget, InAssetTag, InContextTag, InLayerTag});
		UE_LOG(LogArcUI, Verbose, TEXT("CreateWidgetOnLayout - Widget %s from Asset [%s], with context [%s] was added to parent %s on layer [%s]"),
			*NewWidget->GetName(), *InAssetTag.ToString(), *InContextTag.ToString(), *ParentWidget->GetName(), *InLayerTag.ToString());
		return NewWidget;
	}

	UE_LOG(LogArcUI, Warning, TEXT("CreateWidgetOnLayout - No active widget on layer [%s] to create widget from Asset [%s], with context [%s]"),
		*InLayerTag.ToString(), *InAssetTag.ToString(), *InContextTag.ToString());
	return nullptr;
}

void UArcUISubsystem::DestroyWidget(FGameplayTag InAssetTag, FGameplayTag InContextTag, FGameplayTag InLayerTag)
{
	for (int32 ItemIndex = 0; ItemIndex < ManagedWidgets.Num();)
	{
		const auto& Widget = ManagedWidgets[ItemIndex];
		if (Widget.AssetTag == InAssetTag && Widget.ContextTag == InContextTag && Widget.LayerTag == InLayerTag)
		{
			if (auto* Activatable = Cast<UCommonActivatableWidget>(Widget.Widget); Activatable && Layout)
			{
				UE_LOG(LogArcUI, Verbose, TEXT("DestroyWidget - Widget %s removed from layer [%s, %s, %s]"), *Widget.Widget->GetName(), *InAssetTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
				Layout->RemoveWidgetFromLayer(Activatable, InLayerTag);
			}
			else
			{
				UE_LOG(LogArcUI, Verbose, TEXT("DestroyWidget - Widget %s removed from parent [%s, %s, %s]"), *Widget.Widget->GetName(), *InAssetTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
				Widget.Widget->RemoveFromParent();
			}

			ManagedWidgets.RemoveAtSwap(ItemIndex, 1, false);
		}
		else
		{
			++ItemIndex;
		}
	}
}

UUserWidget* UArcUISubsystem::GetActiveWidgetOnLayer(FGameplayTag LayerTag) const
{
	if (Layout)
	{
		return Layout->GetActiveWidgetOnLayer(LayerTag);
	}
	UE_LOG(LogArcUI, Warning, TEXT("No layout"));
	return nullptr;
}

UUserWidget* UArcUISubsystem::GetCreatedWidget(FGameplayTag InAssetTag, FGameplayTag InContextTag, FGameplayTag InLayerTag) const
{
	for (const auto& [Widget, AssetTag, ContextTag, LayerTag] : ManagedWidgets)
	{
		if (AssetTag == InAssetTag && ContextTag == InContextTag && LayerTag == InLayerTag)
		{
			return Widget;
		}
	}
	return nullptr;
}

void UArcUISubsystem::CreateLayout(const ULocalPlayer* LocalPlayer, APlayerController* PlayerController)
{
	if (const auto& LayoutClass = GetGameInstance()->GetSubsystem<UArcUILoader>()->GetLayoutClass())
	{
		Layout = CreateWidget<UArcUILayout>(PlayerController, LayoutClass);
		Layout->SetPlayerContext(FLocalPlayerContext{LocalPlayer});
		Layout->AddToPlayerScreen(1000);

		UE_LOG(LogArcUI, Verbose, TEXT("CreateLayout - Done"));
	}
}

void UArcUISubsystem::DestroyLayout()
{
	Layout->RemoveFromParent();
	Layout = nullptr;

	UE_LOG(LogArcUI, Verbose, TEXT("DestroyLayout - Done"));
}