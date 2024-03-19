// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// ArcUIFramework
#include "ArcUIContext.h"
// UE5
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "Subsystems/GameInstanceSubsystem.h"
// generated
#include "ArcUISubsystem.generated.h"

class UCommonActivatableWidget;
class UArcUIPresenter;
class UArcUILayout;
class ULocalPlayer;
struct FArcUIContextData;

USTRUCT()
struct FArcUIManagedWidget
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> Widget{nullptr};

	FGameplayTag AssetTag;
	FGameplayTag ContextTag;
	FGameplayTag LayerTag;
};


UCLASS()
class ARCUIFRAMEWORK_API UArcUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ~ USubsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void AddContext(UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag);

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void AddContextWithPayload(
		UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag,
		const TInstancedStruct<FArcUIContextPayload>& Payload);

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void AddExclusiveContext(UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag);

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void AddExclusiveContextWithPayload(
		UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag,
		const TInstancedStruct<FArcUIContextPayload>& Payload);
	
	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void RemoveContext(UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag);

	/**
	 * If the context is present, it will be removed
	 * If the context is not present, it will be added
	 * @param ContextTag Tag to check and be added or removed
	 */
	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void ToggleContext(UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag);

	/**
	 * If the context is present, it will be removed
	 * If the context is not present, it will be added
	 * @param ContextTag Tag to check and be added or removed
	 * @param Payload Optional payload data
	 */
	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void ToggleContextWithPayload(
		UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag,
		const TInstancedStruct<FArcUIContextPayload>& Payload);
	
	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	bool HasContext(UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag) const;

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	bool HasPayload(UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag) const;

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	TInstancedStruct<FArcUIContextPayload> GetPayload(UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag) const;

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void BackupContext();

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void RestoreContext();

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void ShowContext(UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag);

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void HideContext(UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag ContextTag);

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void RegisterPresenter(UArcUIPresenter* Presenter);

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	void UnRegisterPresenter(UArcUIPresenter* Presenter);

	virtual void OnPlayerAdded(ULocalPlayer* LocalPlayer);
	virtual void OnPlayerRemoved(ULocalPlayer* LocalPlayer);

	UFUNCTION(BlueprintCallable, Category="UI", BlueprintCosmetic)
	UUserWidget* CreateWidgetOnLayout(
		UPARAM(meta=(Categories = "ArcUI.Asset")) FGameplayTag InAssetTag,
		UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag InContextTag,
		UPARAM(meta=(Categories = "ArcUI.Layer")) FGameplayTag InLayerTag);

	void DestroyWidget(FGameplayTag InAssetTag, FGameplayTag InContextTag, FGameplayTag InLayerTag);

	const UArcUILayout* GetLayout() const { return Layout; }
	
	[[nodiscard]]
	UUserWidget* GetActiveWidgetOnLayer(FGameplayTag LayerTag) const;

	UFUNCTION(BlueprintPure, Category="UI", BlueprintCosmetic)
	UUserWidget* GetCreatedWidget(
		UPARAM(meta=(Categories = "ArcUI.Asset")) FGameplayTag InAssetTag,
		UPARAM(meta=(Categories = "ArcUI.Context")) FGameplayTag InContextTag,
		UPARAM(meta=(Categories = "ArcUI.Layer")) FGameplayTag InLayerTag) const;

protected:
	void CreateLayout(const ULocalPlayer* LocalPlayer, APlayerController* PlayerController);
	void DestroyLayout();

	FGameplayTagContainer ContextTags{};
	FGameplayTagContainer ContextTagsBackup{};

	UPROPERTY(Transient)
	TArray<TObjectPtr<UArcUIPresenter>> Presenters;

	UPROPERTY(Transient)
	TObjectPtr<UArcUILayout> Layout{nullptr};

	UPROPERTY(Transient)
	TArray<FArcUIManagedWidget> ManagedWidgets;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TInstancedStruct<FArcUIContextPayload>> Payloads;

	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> FirstLocalPlayer{nullptr};
};