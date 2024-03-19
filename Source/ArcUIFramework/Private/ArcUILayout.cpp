// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUILayout.h"

// ArcUI
#include "ArcUIDefinitions.h"
#include "ArcUILibrary.h"
#include "ArcUILog.h"
// CommonUI
#include "Widgets/CommonActivatableWidgetContainer.h"
// generated
#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcUILayout)

#if WITH_EDITOR
const FText UArcUILayout::GetPaletteCategory()
{
	return ArcUI::PaletteCategory;
}
#endif

void UArcUILayout::RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (!IsDesignTime())
	{
		LayerWidget->OnTransitioningChanged.AddUObject(this, &ThisClass::OnWidgetStackTransitioning);
		// TODO (Lyra):
		// Consider allowing a transition duration, we currently set it to 0, because if it's not 0, the
		// transition effect will cause focus to not transition properly to the new widgets when using
		// gamepad always.
		LayerWidget->SetTransitionDuration(0.0);

		Layers.Add(LayerTag, LayerWidget);
	}
}

UCommonActivatableWidgetContainerBase* UArcUILayout::GetLayer(FGameplayTag LayerTag) const
{
	return Layers.FindRef(LayerTag);
}

TArray<FGameplayTag> UArcUILayout::GetLayerTags() const
{
	TArray<FGameplayTag> Keys;
	Layers.GetKeys(Keys);
	return Keys;
}

UCommonActivatableWidget* UArcUILayout::GetActiveWidgetOnLayer(FGameplayTag LayerTag) const
{
	if (const auto* Layer = GetLayer(LayerTag))
	{
		if (auto* ActiveWidget = Layer->GetActiveWidget())
		{
			return ActiveWidget;
		}

		const auto& AllWidgets = Layer->GetWidgetList();
		if (AllWidgets.Num() > 0)
		{
			// TODO: find right widget?
			return AllWidgets[0];
		}
	}
	UE_LOG(LogArcUI, Warning, TEXT("GetActiveWidgetOnLayer - No active widget on layer [%s]"), *LayerTag.ToString());
	return nullptr;
}

void UArcUILayout::OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning)
{
	if (bIsTransitioning)
	{
		static const FName SuspendReason{TEXT("GlobalStackTransition")};
		const FName SuspendToken = UArcUILibrary::SuspendInputForPlayer(GetOwningLocalPlayer(), SuspendReason);
		SuspendInputTokens.Add(SuspendToken);
	}
	else
	{
		if (ensure(SuspendInputTokens.Num() > 0))
		{
			const FName SuspendToken = SuspendInputTokens.Pop();
			UArcUILibrary::ResumeInputForPlayer(GetOwningLocalPlayer(), SuspendToken);
		}
	}
}

void UArcUILayout::RemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget, FGameplayTag LayerTag)
{
	if (auto* Layer = GetLayer(LayerTag))
	{
		Layer->RemoveWidget(*ActivatableWidget);
	}
}
