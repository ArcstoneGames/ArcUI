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
	UE_CLOG(!bHandled, LogArcUI, Warning, TEXT("OnContextAdded on Tag %s not handled by %s"), *ContextTag.ToString(), *GetNameSafe(this));
}

void UArcUIPresenter::OnContextRemoved(FGameplayTag ContextTag)
{
	const bool bHandled = HandleOnContextRemoved(ContextTag);
	UE_CLOG(!bHandled, LogArcUI, Warning, TEXT("OnContextRemoved on Tag %s not handled by %s"), *ContextTag.ToString(), *GetNameSafe(this));
}

void UArcUIPresenter::ShowContext(FGameplayTag ContextTag)
{
	const bool bHandled = HandleShowContext(ContextTag);
	UE_CLOG(!bHandled, LogArcUI, Warning, TEXT("ShowContext on Tag %s not handled by %s"), *ContextTag.ToString(), *GetNameSafe(this));
}

void UArcUIPresenter::HideContext(FGameplayTag ContextTag)
{
	const bool bHandled = HandleHideContext(ContextTag);
	UE_CLOG(!bHandled, LogArcUI, Warning, TEXT("HideContext on Tag %s not handled by %s"), *ContextTag.ToString(), *GetNameSafe(this));
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
