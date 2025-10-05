// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUIPresenterComponent.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"

// ArcUI
#include "ArcUISubsystem.h"
// generated

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcUIPresenterComponent)

UArcUIPresenterComponent::UArcUIPresenterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UArcUIPresenterComponent::RegisterPresenter(UArcUIPresenter* Presenter)
{
	if (ensure(Presenter))
	{
		UWorld* World = GetWorld();
		World->GetGameInstance()->GetSubsystem<UArcUISubsystem>()->RegisterPresenter(Presenter);
	}
}

void UArcUIPresenterComponent::UnRegisterPresenter(UArcUIPresenter* Presenter)
{
	if (Presenter)
	{
		UWorld* World = GetWorld();
		World->GetGameInstance()->GetSubsystem<UArcUISubsystem>()->UnRegisterPresenter(Presenter);
	}
}
