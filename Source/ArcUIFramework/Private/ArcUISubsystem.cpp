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

void UArcUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GetGameInstance()->OnLocalPlayerAddedEvent.AddUObject(this, &ThisClass::OnPlayerAdded);
	GetGameInstance()->OnLocalPlayerRemovedEvent.AddUObject(this, &ThisClass::OnPlayerRemoved);
}

void UArcUISubsystem::AddContext(FGameplayTag ContextTag)
{
	AddContextWithPayload(ContextTag, {});
}

void UArcUISubsystem::AddContextWithPayload(FGameplayTag ContextTag, const TInstancedStruct<FArcUIContextPayload>& Payload)
{
	if (!ensureMsgf(!ContextTags.HasTagExact(ContextTag), TEXT("AddContext - context already added: %s"), *ContextTag.ToString()))
	{
		return;
	}

	ContextTags.AddTag(ContextTag);

	if (Payload.IsValid())
	{
		ensureMsgf(nullptr == Payloads.Find(ContextTag), TEXT("AddContext - A payload was already set for this context: %s"), *ContextTag.ToString());
		Payloads.Add(ContextTag, Payload);
		UE_LOG(LogArcUI, Verbose, TEXT("AddContext: %s with Payload %s"), *ContextTag.ToString(), *Payload.GetScriptStruct()->GetFullName());
	}
	else
	{
		UE_LOG(LogArcUI, Verbose, TEXT("AddContext: %s"), *ContextTag.ToString());
	}
	
	GetGameInstance()->GetSubsystem<UArcUILoader>()->OnContextAdded(ContextTag);

	for (const auto& Presenter : Presenters)
	{
		if (ensureMsgf(Presenter, TEXT("ArcUISubsystem: stale registered presenter, make sure to call UnRegisterPresenter")) &&
			Presenter->HasContextTag(ContextTag))
		{
			Presenter->OnContextAdded(ContextTag, Payload);
		}
	}
}

void UArcUISubsystem::AddExclusiveContext(FGameplayTag ContextTag)
{
	AddExclusiveContextWithPayload(ContextTag, {});
}

void UArcUISubsystem::AddExclusiveContextWithPayload(FGameplayTag ContextTag, const TInstancedStruct<FArcUIContextPayload>& Payload)
{
	while (!ContextTags.IsEmpty())
	{
		RemoveContext(ContextTags.First());
	}

	ContextTags.Reset();

	AddContextWithPayload(ContextTag, Payload);
}

void UArcUISubsystem::RemoveContext(FGameplayTag ContextTag)
{
	if (!ensureMsgf(ContextTags.HasTagExact(ContextTag), TEXT("RemoveContext - context already removed or never added: %s"), *ContextTag.ToString()))
	{
		return;
	}

	ContextTags.RemoveTag(ContextTag);

	const int32 RemovedCount = Payloads.Remove(ContextTag);
	UE_CLOG(RemovedCount == 0, LogArcUI, Verbose, TEXT("RemoveContext: %s"), *ContextTag.ToString());
	UE_CLOG(RemovedCount >= 0, LogArcUI, Verbose, TEXT("RemoveContext: %s with associated payload"), *ContextTag.ToString());
	
	for (const auto& Presenter : Presenters)
	{
		if (ensureMsgf(Presenter, TEXT("ArcUISubsystem: stale registered presenter, make sure to call UnRegisterPresenter")) &&
			Presenter->HasContextTag(ContextTag))
		{
			Presenter->OnContextRemoved(ContextTag);
		}
	}
	
	GetGameInstance()->GetSubsystem<UArcUILoader>()->OnContextRemoved(ContextTag);
}

void UArcUISubsystem::ToggleContext(FGameplayTag ContextTag)
{
	ToggleContextWithPayload(ContextTag, {});
}

void UArcUISubsystem::ToggleContextWithPayload(FGameplayTag ContextTag,	const TInstancedStruct<FArcUIContextPayload>& Payload)
{
	if (HasContext(ContextTag))
	{
		RemoveContext(ContextTag);
	}
	else
	{
		AddContextWithPayload(ContextTag, Payload);
	}
}

bool UArcUISubsystem::HasContext(FGameplayTag ContextTag) const
{
	return ContextTags.HasTagExact(ContextTag);
}

bool UArcUISubsystem::HasPayload(FGameplayTag ContextTag) const
{
	return nullptr != Payloads.Find(ContextTag);
}

TInstancedStruct<FArcUIContextPayload> UArcUISubsystem::GetPayload(FGameplayTag ContextTag) const
{
	if (const auto* Payload = Payloads.Find(ContextTag))
	{
		return *Payload;
	}
	
	return {};
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
		if (const auto* Payload = Payloads.Find(ContextTag))
		{
			AddContextWithPayload(ContextTag, *Payload);
		}
		else
		{
			AddContext(ContextTag);	
		}
	}

	ContextTagsBackup.Reset();
}

void UArcUISubsystem::ShowContext(FGameplayTag ContextTag)
{
	if (!ensureMsgf(!ContextTags.HasTagExact(ContextTag), TEXT("AddContext - context already added: %s"), *ContextTag.ToString()))
	{
		return;
	}

	for (const auto& Presenter : Presenters)
	{
		if (ensureMsgf(Presenter, TEXT("ArcUISubsystem: stale registered presenter, make sure to call UnRegisterPresenter")) &&
			Presenter->HasContextTag(ContextTag))
		{
			Presenter->ShowContext(ContextTag);
		}
	}
}

void UArcUISubsystem::HideContext(FGameplayTag ContextTag)
{
	if (!ensureMsgf(!ContextTags.HasTagExact(ContextTag), TEXT("AddContext - context already added: %s"), *ContextTag.ToString()))
	{
		return;
	}

	for (const auto& Presenter : Presenters)
	{
		if (ensureMsgf(Presenter, TEXT("ArcUISubsystem: stale registered presenter, make sure to call UnRegisterPresenter")) &&
			Presenter->HasContextTag(ContextTag))
		{
			Presenter->HideContext(ContextTag);
		}
	}
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
		if (Presenter->HasContextTag(ContextTag))
		{
			if (const auto* Payload = Payloads.Find(ContextTag))
			{
				Presenter->OnContextAdded(ContextTag, *Payload);
			}
			else
			{
				Presenter->OnContextAdded(ContextTag, {});
			}
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
	if (!Layout && nullptr == FirstLocalPlayer)
	{
		FirstLocalPlayer = LocalPlayer;
		if (auto* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
		{
			CreateLayout(LocalPlayer, PlayerController);
		}
		else
		{
			LocalPlayer->OnPlayerControllerChanged().AddLambda([this](APlayerController* PlayerController)
			{
				CreateLayout(FirstLocalPlayer, PlayerController);
			});
		}
	}
}

void UArcUISubsystem::OnPlayerRemoved(ULocalPlayer* LocalPlayer)
{
	if (Layout && Layout->GetPlayerContext().GetLocalPlayer() == LocalPlayer && FirstLocalPlayer == LocalPlayer)
	{
		DestroyLayout();
		FirstLocalPlayer = nullptr;
	}
}

UUserWidget* UArcUISubsystem::CreateWidgetOnLayout(FGameplayTag InViewTag, FGameplayTag InContextTag, FGameplayTag InLayerTag, FName SlotName/* = NAME_None*/)
{
	if (!Layout)
	{
		UE_LOG(LogArcUI, Warning, TEXT("CreateWidgetOnLayout - No layout to create widget on, from View [%s], with context [%s] on layer [%s]"),
			*InViewTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
		return nullptr;
	}

	const TSubclassOf<UUserWidget> WidgetClass = GetGameInstance()->GetSubsystem<UArcUILoader>()->GetWidgetClass<UUserWidget>(InViewTag, InContextTag);
	if (WidgetClass.Get() == nullptr)
	{
		UE_LOG(LogArcUI, Warning, TEXT("CreateWidgetOnLayout - Could not create widget from View [%s], with context [%s] on layer [%s]"),
			*InViewTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
		return nullptr;
	}
	
	if (const TSubclassOf<UCommonActivatableWidget> ActivatableClass{WidgetClass})
	{
		auto* NewWidget = Layout->PushWidgetToLayer(InLayerTag, ActivatableClass);
		ManagedWidgets.Add({NewWidget, InViewTag, InContextTag, InLayerTag});
		UE_LOG(LogArcUI, Verbose, TEXT("CreateWidgetOnLayout - Widget %s from View [%s], with context [%s] was pushed on layer [%s]"),
			*NewWidget->GetName(), *InViewTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
		return NewWidget;
	}

	UUserWidget* NewWidget{nullptr};
	if (auto* ParentWidget = Layout->GetActiveWidgetOnLayer(InLayerTag))
	{
		NewWidget = CreateWidget<UUserWidget>(ParentWidget, WidgetClass);

		if (SlotName.IsNone())
		{
			NewWidget->AddToPlayerScreen();
		}
		else
		{
			ParentWidget->SetContentForSlot(SlotName, NewWidget);
		}

		ManagedWidgets.Add({NewWidget, InViewTag, InContextTag, InLayerTag});
		UE_LOG(LogArcUI, Verbose, TEXT("CreateWidgetOnLayout - Widget %s from View [%s], with context [%s] was added to parent %s on layer [%s]"),
			*NewWidget->GetName(), *InViewTag.ToString(), *InContextTag.ToString(), *ParentWidget->GetName(), *InLayerTag.ToString());
		return NewWidget;
	}

	UE_LOG(LogArcUI, Warning, TEXT("CreateWidgetOnLayout - No active widget on layer [%s] to create widget from View [%s], with context [%s]"),
		*InLayerTag.ToString(), *InViewTag.ToString(), *InContextTag.ToString());
	return nullptr;
}

void UArcUISubsystem::DestroyWidget(FGameplayTag InViewTag, FGameplayTag InContextTag, FGameplayTag InLayerTag)
{
	for (int32 ItemIndex = 0; ItemIndex < ManagedWidgets.Num();)
	{
		const auto& Widget = ManagedWidgets[ItemIndex];
		if (Widget.ViewTag == InViewTag && Widget.ContextTag == InContextTag && Widget.LayerTag == InLayerTag)
		{
			if (auto* Activatable = Cast<UCommonActivatableWidget>(Widget.Widget); Activatable && Layout)
			{
				UE_LOG(LogArcUI, Verbose, TEXT("DestroyWidget - Widget %s removed from layer [%s, %s, %s]"), *Widget.Widget->GetName(), *InViewTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
				Layout->RemoveWidgetFromLayer(Activatable, InLayerTag);
			}
			else
			{
				UE_LOG(LogArcUI, Verbose, TEXT("DestroyWidget - Widget %s removed from parent [%s, %s, %s]"), *Widget.Widget->GetName(), *InViewTag.ToString(), *InContextTag.ToString(), *InLayerTag.ToString());
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

UUserWidget* UArcUISubsystem::GetActiveWidgetOnLayer_Impl(FGameplayTag LayerTag) const
{
	if (Layout)
	{
		return Layout->GetActiveWidgetOnLayer(LayerTag);
	}
	UE_LOG(LogArcUI, Warning, TEXT("No layout"));
	return nullptr;
}

UUserWidget* UArcUISubsystem::GetCreatedWidget(FGameplayTag InViewTag, FGameplayTag InContextTag, FGameplayTag InLayerTag) const
{
	for (const auto& [Widget, ViewTag, ContextTag, LayerTag] : ManagedWidgets)
	{
		if (ViewTag == InViewTag && ContextTag == InContextTag && LayerTag == InLayerTag)
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
