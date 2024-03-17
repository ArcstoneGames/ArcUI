// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// ArcUI
#include "ArcUIAsset.h"
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
 * You shouldn't need to call it manually unless you want to load assets on demand or beyond contexts
 */
UCLASS(Config = Game)
class ARCUIFRAMEWORK_API UArcUILoader : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	template <typename WidgetClass> [[nodiscard]]
	TSubclassOf<WidgetClass> GetAssetClass(FGameplayTag AssetTag, FGameplayTag ContextTag);
	
	TSubclassOf<UArcUILayout> GetLayoutClass() const { return LayoutClass; }

	void OnContextAdded(FGameplayTag ContextTag);
	void OnContextRemoved(FGameplayTag ContextTag);

protected:
	[[nodiscard]]
	UClass* GetAssetClass_Internal(FGameplayTag AssetTag, FGameplayTag ContextTag);

	void CreatePresenter(FGameplayTag ContextTag, const FArcUIPresenterInfo& PresenterInfo);

	// Override this method to manage loaded presenters subclasses
	virtual TSubclassOf<UArcUIPresenter> LoadPresenterSubclass(FGameplayTag AssetTag, const TSoftClassPtr<UArcUIPresenter>& AssetPointer);

	// Override this if you manage loaded widgets subclasses
	virtual TSubclassOf<UArcUIPresenter> GetLoadedPresenterSubclass(FGameplayTag AssetTag);
	
	// Override this method to manage loaded widgets subclasses
	virtual TSubclassOf<UUserWidget> LoadWidgetSubclass(FGameplayTag AssetTag, const TSoftClassPtr<UUserWidget>& AssetPointer);

	// Override this if you manage loaded widgets subclasses
	virtual TSubclassOf<UUserWidget> GetLoadedWidgetSubclass(FGameplayTag AssetTag);

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
TSubclassOf<WidgetClass> UArcUILoader::GetAssetClass(FGameplayTag AssetTag, FGameplayTag ContextTag)
{
	static_assert(TIsDerivedFrom<WidgetClass, UUserWidget>::IsDerived, "GetAssetClass can only work with UUserWidget classes");

	return GetAssetClass_Internal(AssetTag, ContextTag);
}
