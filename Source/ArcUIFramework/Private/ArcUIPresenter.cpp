// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUIPresenter.h"

#include "ArcUILog.h"
#include "ArcUISubsystem.h"

bool UArcUIPresenter::HasContextTag(FGameplayTag ContextTag) const
{
	return ContextTags.HasTag(ContextTag);
}

void UArcUIPresenter::OnContextAdded(FGameplayTag ContextTag, const TInstancedStruct<FArcUIContextPayload>& Payload)
{
	const bool bHandled = HandleOnContextAdded(ContextTag, Payload);
	UE_CLOG(!bHandled, LogArcUI, Verbose, TEXT("OnContextAdded on Tag %s not handled natively by %s"), *ContextTag.ToString(), *GetNameSafe(this));
	BP_HandleOnContextAdded(ContextTag, Payload);
}

void UArcUIPresenter::OnContextRemoved(FGameplayTag ContextTag)
{
	const bool bHandled = HandleOnContextRemoved(ContextTag);
	UE_CLOG(!bHandled, LogArcUI, Verbose, TEXT("OnContextRemoved on Tag %s not handled natively by %s"), *ContextTag.ToString(), *GetNameSafe(this));
	BP_HandleOnContextRemoved(ContextTag);
}

void UArcUIPresenter::ShowContext(FGameplayTag ContextTag)
{
	const bool bHandled = HandleShowContext(ContextTag);
	UE_CLOG(!bHandled, LogArcUI, Verbose, TEXT("ShowContext on Tag %s not handled natively by %s"), *ContextTag.ToString(), *GetNameSafe(this));
	BP_HandleShowContext(ContextTag);
}

void UArcUIPresenter::HideContext(FGameplayTag ContextTag)
{
	const bool bHandled = HandleHideContext(ContextTag);
	UE_CLOG(!bHandled, LogArcUI, Verbose, TEXT("HideContext on Tag %s not handled natively by %s"), *ContextTag.ToString(), *GetNameSafe(this));
	BP_HandleHideContext(ContextTag);
}

UGameInstance* UArcUIPresenter::GetGameInstance() const
{
	if (UISubsystem)
	{
		return UISubsystem->GetGameInstance();
	}
	if (const UWorld* World = GetWorld())
	{
		return World->GetGameInstance();
	}
	return nullptr;
}

UWorld* UArcUIPresenter::GetWorld() const
{
	if (UISubsystem)
	{
		return UISubsystem->GetWorld();
	}
	return Super::GetWorld();
}
