// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "Engine/DeveloperSettings.h"
// generated
#include "ArcUISettings.generated.h"

class UArcUILayout;
struct FArcUIAsset;
struct FArcUIPresenterInfo;

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="ArcUI Settings"))
class ARCUIFRAMEWORK_API UArcUISettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Unique game UI layout */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Layout")
	TSoftClassPtr<UArcUILayout> LayoutClass{nullptr};

	/** Static list of UI Assets */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TArray<FArcUIAsset> Assets;

	/** Registry to use for UI Assets */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category= "Assets")
	FName AssetsRegistryName{TEXT("ArcUI_AssetsRegistry")};

	/** Static list of UI Presenters */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Presenters")
	TArray<FArcUIPresenterInfo> Presenters;

	/** Registry to use for UI Presenters */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category= "Presenters")
	FName PresentersRegistryName{TEXT("ArcUI_PresentersRegistry")};

	virtual FName GetCategoryName() const override { return FName{TEXT("Arcstone")}; }
};
