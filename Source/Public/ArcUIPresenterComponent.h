// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "Components/ActorComponent.h"
// generated
#include "ArcUIPresenterComponent.generated.h"

class UArcUIPresenter;

/**
 * Parent class for UI Presenter holding actor components
 */
UCLASS(Abstract)
class ARCUI_API UArcUIPresenterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UArcUIPresenterComponent();

	void RegisterPresenter(UArcUIPresenter* Presenter);
	void UnRegisterPresenter(UArcUIPresenter* Presenter);
};
