// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// ArcUI
#include "ArcUIViewInfo.h"
// UE5
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
// generated
#include "ArcUILoader.generated.h"

struct FArcUIPresenterInfo;
class UArcUIPresenter;

USTRUCT()
struct FArcUIManagedPresenters
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<TObjectPtr<UArcUIPresenter>> Presenters;
};

/* 
 * The UI Loader is the unique point of access for Widget resources
 * You shouldn't need to call it manually unless you want to load widget classes on demand or beyond contexts
 */
UCLASS(Config = Game)
class ARCUIFRAMEWORK_API UArcUILoader : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	template <typename WidgetClass> [[nodiscard]]
	TSubclassOf<WidgetClass> GetWidgetClass(FGameplayTag ViewTag, FGameplayTag ContextTag);
	
	TSubclassOf<UArcUILayout> GetLayoutClass() const { return LayoutClass; }

	void OnContextAdded(FGameplayTag ContextTag);
	void OnContextRemoved(FGameplayTag ContextTag);

protected:
	[[nodiscard]]
	UClass* GetWidgetClass_Internal(FGameplayTag ViewTag, FGameplayTag ContextTag);

	void CreatePresenter(FGameplayTag ContextTag, const FArcUIPresenterInfo& PresenterInfo);

	// Override this method to manage loaded presenters subclasses
	virtual TSubclassOf<UArcUIPresenter> LoadPresenterSubclass(FGameplayTag ViewTag, const TSoftClassPtr<UArcUIPresenter>& AssetPointer);

	// Override this if you manage loaded widgets subclasses
	virtual TSubclassOf<UArcUIPresenter> GetLoadedPresenterSubclass(FGameplayTag ViewTag);
	
	// Override this method to manage loaded widgets subclasses
	virtual TSubclassOf<UUserWidget> LoadWidgetSubclass(FGameplayTag ViewTag, const TSoftClassPtr<UUserWidget>& AssetPointer);

	// Override this if you manage loaded widgets subclasses
	virtual TSubclassOf<UUserWidget> GetLoadedWidgetSubclass(FGameplayTag ViewTag);

	UPROPERTY(Transient)
	TSubclassOf<UArcUILayout> LayoutClass{nullptr};

	UPROPERTY(Transient)
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> ManagedWidgetClasses;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TSubclassOf<UArcUIPresenter>> ManagedPresenterClasses;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FArcUIManagedPresenters> ManagedPresenters;
};


template <typename WidgetClass>
TSubclassOf<WidgetClass> UArcUILoader::GetWidgetClass(FGameplayTag ViewTag, FGameplayTag ContextTag)
{
	static_assert(TIsDerivedFrom<WidgetClass, UUserWidget>::IsDerived, "GetWidgetClass can only work with UUserWidget classes");

	return GetWidgetClass_Internal(ViewTag, ContextTag);
}
