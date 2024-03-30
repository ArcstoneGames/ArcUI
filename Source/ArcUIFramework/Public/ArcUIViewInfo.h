// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
// generated
#include "ArcUIViewInfo.generated.h"

class UArcUILayout;

USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIContextAction final
{
	GENERATED_BODY()

	/** Action context */
	UPROPERTY(EditAnywhere, meta=(Categories="ArcUI.Context"))
	FGameplayTag ContextTag;

	/** Should this view be loaded when its context is added */
	UPROPERTY(EditAnywhere)
	bool bLoadingTiedToContext{false};

	/** Should this view be created when its context is added */
	UPROPERTY(EditAnywhere)
	bool bCreationTiedToContext{false};

	/** On which layer to create the view's widget. None means outside of layers */
	UPROPERTY(EditAnywhere, meta=(Categories="ArcUI.Layer", EditCondition="bCreationTiedToContext"))
	FGameplayTag CreateOnLayerTag;

	/** On which slot to add the view's widget. None means placed on screen */
	UPROPERTY(EditAnywhere, meta=(EditCondition="bCreationTiedToContext"))
	FName SlotName;
};


USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIViewInfo final : public FTableRowBase
{
	GENERATED_BODY()

	/** Tag identifying this view */
	UPROPERTY(EditAnywhere, Category=Widget, meta=(Categories="ArcUI.View"))
	FGameplayTag ViewTag;

	/** Class of the widget */
	UPROPERTY(EditAnywhere, Category=Widget)
	TSoftClassPtr<UUserWidget> WidgetClass{nullptr};

	/** This view is often used and should be loaded at init and never unloaded */
	UPROPERTY(EditAnywhere, Category=Widget)
	bool bKeepAlwaysLoaded{false};

	/** Automatic creation patterns when contexts are requested */
	UPROPERTY(EditAnywhere, Category=Context, meta=(TitleProperty="ContextTag"))
	TArray<FArcUIContextAction> ContextActions;
};
