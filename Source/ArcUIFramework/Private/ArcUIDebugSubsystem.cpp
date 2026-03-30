// Copyright (C) Fabien Poupineau. All Rights Reserved.

// ArcUI
#include "ArcUIDebugSubsystem.h"
#include "ArcUISubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcUIDebugSubsystem)

bool UArcUIDebugSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if UE_BUILD_SHIPPING
	return false;
#else
	return !CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance();
#endif
}

#if !UE_BUILD_SHIPPING

static UArcUISubsystem* FindArcUISubsystem(const UGameInstance* GameInstance)
{
	// UArcUISubsystem::ShouldCreateSubsystem returns false when a subclass exists,
	// so GetSubsystem<UArcUISubsystem>() won't find the concrete subclass by exact key.
	// Iterate subsystems to find any instance that is (or derives from) UArcUISubsystem.
	for (UGameInstanceSubsystem* Subsystem : GameInstance->GetSubsystemArrayCopy<UGameInstanceSubsystem>())
	{
		if (auto* ArcUI = Cast<UArcUISubsystem>(Subsystem))
		{
			return ArcUI;
		}
	}
	return nullptr;
}

void UArcUIDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Ensure the concrete UArcUISubsystem subclass is initialized before us.
	TArray<UClass*> DerivedClasses;
	GetDerivedClasses(UArcUISubsystem::StaticClass(), DerivedClasses, false);
	Collection.InitializeDependency(DerivedClasses.IsEmpty() ? UArcUISubsystem::StaticClass() : DerivedClasses[0]);

	if (auto* UISubsystem = FindArcUISubsystem(GetGameInstance()))
	{
		ContextAddedHandle     = UISubsystem->OnDebugContextAdded.AddUObject(this, &ThisClass::OnContextAdded);
		ContextRemovedHandle   = UISubsystem->OnDebugContextRemoved.AddUObject(this, &ThisClass::OnContextRemoved);
		PresenterChangedHandle = UISubsystem->OnDebugPresenterChanged.AddUObject(this, &ThisClass::OnStateChanged);
		WidgetChangedHandle    = UISubsystem->OnDebugWidgetChanged.AddUObject(this, &ThisClass::OnStateChanged);
	}
}

void UArcUIDebugSubsystem::Deinitialize()
{
	if (auto* UISubsystem = FindArcUISubsystem(GetGameInstance()))
	{
		UISubsystem->OnDebugContextAdded.Remove(ContextAddedHandle);
		UISubsystem->OnDebugContextRemoved.Remove(ContextRemovedHandle);
		UISubsystem->OnDebugPresenterChanged.Remove(PresenterChangedHandle);
		UISubsystem->OnDebugWidgetChanged.Remove(WidgetChangedHandle);
	}

	Super::Deinitialize();
}

void UArcUIDebugSubsystem::OnContextAdded(FGameplayTag ContextTag, const TInstancedStruct<FArcUIContextPayload>& Payload)
{
	FString Entry = FString::Printf(TEXT("[+] %s"), *ContextTag.ToString());
	if (Payload.IsValid())
	{
		Entry += FString::Printf(TEXT("  payload: %s"), *Payload.GetScriptStruct()->GetName());
	}
	ContextLog.Add(MoveTemp(Entry));
	OnDebugStateChanged.Broadcast();
}

void UArcUIDebugSubsystem::OnContextRemoved(FGameplayTag ContextTag)
{
	ContextLog.Add(FString::Printf(TEXT("[-] %s"), *ContextTag.ToString()));
	OnDebugStateChanged.Broadcast();
}

void UArcUIDebugSubsystem::OnStateChanged()
{
	OnDebugStateChanged.Broadcast();
}

#else

void UArcUIDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UArcUIDebugSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

#endif // !UE_BUILD_SHIPPING
