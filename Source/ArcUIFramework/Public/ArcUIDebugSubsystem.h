// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// ArcUI
#include "ArcUIContext.h"
// UE
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "Subsystems/GameInstanceSubsystem.h"
// generated
#include "ArcUIDebugSubsystem.generated.h"

/**
 * Collects ArcUI runtime events (context, widget, presenter changes) during PIE.
 * Stripped in Shipping builds via ShouldCreateSubsystem.
 * The editor debug tab reads from this subsystem via GEditor->PlayWorld.
 */
UCLASS()
class ARCUIFRAMEWORK_API UArcUIDebugSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

#if !UE_BUILD_SHIPPING
	/** Fires whenever any ArcUI state changes — subscribe to drive editor panels. */
	FSimpleMulticastDelegate OnDebugStateChanged;

	const TArray<FString>& GetContextLog() const { return ContextLog; }

private:
	void OnContextAdded(FGameplayTag ContextTag, const TInstancedStruct<FArcUIContextPayload>& Payload);
	void OnContextRemoved(FGameplayTag ContextTag);
	void OnStateChanged();

	TArray<FString> ContextLog;
	FDelegateHandle ContextAddedHandle;
	FDelegateHandle ContextRemovedHandle;
	FDelegateHandle PresenterChangedHandle;
	FDelegateHandle WidgetChangedHandle;
#endif
};
