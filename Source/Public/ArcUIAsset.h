// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "GameplayTagContainer.h"
// generated
#include "ArcUIAsset.generated.h"

class UArcUILayout;

USTRUCT(BlueprintType)
struct ARCUI_API FArcUIContextAction final
{
	GENERATED_BODY()

	/** Available contexts for this asset */
	UPROPERTY(EditAnywhere, meta=(Categories="ArcUI.Context"))
	FGameplayTag ContextTag;

	/** Should this asset be loaded when its context is added */
	UPROPERTY(EditAnywhere)
	bool bLoadingTiedToContext{false};

	/** Should this asset be created when its context is added */
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bCreationTiedToContext{false};

	/** On which layer to create the asset. None means outside of layers */
	UPROPERTY(EditAnywhere, meta=(Categories="ArcUI.Layer", EditCondition="bCreationTiedToContext"))
	FGameplayTag CreateOnLayerTag;
};


USTRUCT(BlueprintType)
struct ARCUI_API FArcUIAsset final : public FTableRowBase
{
	GENERATED_BODY()

	/** Tag identifying this asset */
	UPROPERTY(EditAnywhere, meta=(Categories="ArcUI.Asset"))
	FGameplayTag AssetTag;

	/** Class of the widget asset */
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UUserWidget> WidgetClass{nullptr};

	/** This asset is often used and should be loaded at init and never unloaded */
	UPROPERTY(EditAnywhere)
	bool bKeepAlwaysLoaded{false};

	/** Automatic creation patterns when context is requested */
	UPROPERTY(EditAnywhere, meta=(TitleProperty="ContextTag"))
	TArray<FArcUIContextAction> ContextActions;
};
