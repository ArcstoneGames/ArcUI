// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
// generated
#include "ArcUIAsset.generated.h"

class UArcUILayout;

USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIContextAction final
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

	/** On which slot to add the widget. None means placed on screen */
	UPROPERTY(EditAnywhere, meta=(EditCondition="bCreationTiedToContext"))
	FName SlotName;
};


USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIAsset final : public FTableRowBase
{
	GENERATED_BODY()

	/** Tag identifying this asset */
	UPROPERTY(EditAnywhere, Category=Widget, meta=(Categories="ArcUI.Asset"))
	FGameplayTag AssetTag;

	/** Class of the widget asset */
	UPROPERTY(EditAnywhere, Category=Widget)
	TSoftClassPtr<UUserWidget> WidgetClass{nullptr};

	/** This asset is often used and should be loaded at init and never unloaded */
	UPROPERTY(EditAnywhere, Category=Widget)
	bool bKeepAlwaysLoaded{false};

	/** Automatic creation patterns when contexts are requested */
	UPROPERTY(EditAnywhere, Category=Context, meta=(TitleProperty="ContextTag"))
	TArray<FArcUIContextAction> ContextActions;
};
