// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUIPresenterComponent.h"

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
		GetWorld()->GetGameInstance()->GetSubsystem<UArcUISubsystem>()->RegisterPresenter(Presenter);
	}
}

void UArcUIPresenterComponent::UnRegisterPresenter(UArcUIPresenter* Presenter)
{
	if (Presenter)
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UArcUISubsystem>()->UnRegisterPresenter(Presenter);
	}
}
