// Copyright (C) Fabien Poupineau. All Rights Reserved.
// Heavily inspired from the Lyra game

#pragma once

// CommonUI
#include "CommonUserWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
// UE5
#include "GameplayTagContainer.h"
#include "Engine/AssetManager.h"
// generated
#include "ArcUILayout.generated.h"

class UCommonActivatableWidget;

/**
 * The UI layout of your game.  This widget class represents how to layout, push and display all layers
 * of the UI for a single player.  Each player in a split-screen game will receive their own primary game layout.
 */
UCLASS(Abstract, ClassGroup = UI, meta = (Category = "ArcUI", DisableNativeTick))
// ReSharper disable once CppClassCanBeFinal
class ARCUIFRAMEWORK_API UArcUILayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	/** Register a layer that widgets can be pushed onto. */
	UFUNCTION(BlueprintCallable, Category="Layer")
	void RegisterLayer(UPARAM(meta = (Categories = "ArcUI.Layer")) FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

	[[nodiscard]]
	// Get the layer widget for the given layer tag.
	UCommonActivatableWidgetContainerBase* GetLayer(FGameplayTag LayerTag) const;

	[[nodiscard]]
	// Get the pushed widget for the given layer tag.
	UCommonActivatableWidget* GetActiveWidgetOnLayer(FGameplayTag LayerTag) const; 

	void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning);

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayer(FGameplayTag LayerTag, TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (auto* Layer = GetLayer(LayerTag))
		{
			return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}

		return nullptr;
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayer(FGameplayTag LayerTag, TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass)
	{
		return PushWidgetToLayer<ActivatableWidgetT>(LayerTag, ActivatableWidgetClass, [](ActivatableWidgetT&){});
	}

	void RemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget, FGameplayTag LayerTag);

private:
	// Lets us keep track of all suspended input tokens so that multiple async UIs can be loading and we correctly suspend
	// for the duration of all of them.
	TArray<FName> SuspendInputTokens;

	// The registered layers for the primary layout.
	UPROPERTY(Transient, meta = (Categories = "ArcUI.Layer"))
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> Layers;
};
