// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "Engine/DeveloperSettings.h"
// generated
#include "ArcUISettings.generated.h"

class UArcUILayout;

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="ArcUI Settings"))
class ARCUIFRAMEWORK_API UArcUISettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Unique game UI layout */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	TSoftClassPtr<UArcUILayout> LayoutClass{nullptr};

	/** Registry name */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category= "Assets")
	FName RegistryName{TEXT("ArcUI_AssetsRegistry")};
};
