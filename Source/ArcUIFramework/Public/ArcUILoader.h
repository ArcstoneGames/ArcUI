// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// ArcUI
#include "ArcUIAsset.h"
// UE5
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
// generated
#include "ArcUILoader.generated.h"

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
	
	// Override this method to manage loaded subclasses
	virtual TSubclassOf<UUserWidget> LoadSubclass(FGameplayTag AssetTag, const TSoftClassPtr<UUserWidget>& AssetPointer);

	// Override this if you manage loaded subclasses
	virtual TSubclassOf<UUserWidget> GetLoadedSubclass(FGameplayTag AssetTag);

	UPROPERTY(Transient)
	TSubclassOf<UArcUILayout> LayoutClass{nullptr};

	UPROPERTY(Transient)
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> ManagedClasses;
};


template <typename WidgetClass>
TSubclassOf<WidgetClass> UArcUILoader::GetAssetClass(FGameplayTag AssetTag, FGameplayTag ContextTag)
{
	static_assert(TIsDerivedFrom<WidgetClass, UUserWidget>::IsDerived, "GetAssetClass can only work with UUserWidget classes");

	return GetAssetClass_Internal(AssetTag, ContextTag);
}
