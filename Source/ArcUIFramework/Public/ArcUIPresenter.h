// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// ArcUI
#include "ArcUIContext.h"
// UE5
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "Engine/DataTable.h"
// generated
#include "ArcUIPresenter.generated.h"

class UArcUISubsystem;
class UUserWidget;
class AActor;

/**
 * Base class for (standalone) Presenters
 */
UCLASS(Abstract, Blueprintable, DisplayName="UI Presenter", meta = (ShowWorldContextPin))
class ARCUIFRAMEWORK_API UArcUIPresenter : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief Retrieve the Context tags for this presenter
	 * @return The context tags affecting the presenter's widgets display
	 */
	const FGameplayTagContainer& GetContextTags() const { return ContextTags; }

	/**
	 * @brief Set a single Context tag for this presenter
	 * @param InContextTag Single context tag affecting the presenter's widgets display
	 */
	void SetContextTag(FGameplayTag InContextTag) { ContextTags = FGameplayTagContainer{InContextTag}; }
	
	/**
	 * @brief Set the Context tags for this presenter
	 * @param InContextTags Context Tags affecting the presenter's widgets display
	 */
	void SetContextTags(const FGameplayTagContainer& InContextTags) { ContextTags = InContextTags; }

	/**
	 * @brief Test if the Context is handled by the Presenter
	 * @param ContextTag Context to test
	 * @return True if handled
	 */
	bool HasContextTag(FGameplayTag ContextTag) const;
	
	/**
	 * @brief First entry point for the Presenter to prepare anything
	 * @param Actor Owner actor
	 */
	virtual void SetObservedActor(AActor* Actor) { ObservedActor = Actor; }

	/**
	 * @brief In case the widget is managed externally, the presenter can receive a widget to work with
	 * @param InWidget Assigned widget to work with (can be nullptr if self handled)
	 */
	virtual void AssignWidget(UUserWidget* InWidget) { Widget = InWidget; }
	
	/**
	 * @brief Called when a context is added
	 * @param ContextTag Tag being added to the context
	 */
	void OnContextAdded(FGameplayTag ContextTag, const TInstancedStruct<FArcUIContextPayload>& Payload);

	/**
	 * @brief Called when a context is removed
	 * @param ContextTag Tag being removed from the context
	 */
	void OnContextRemoved(FGameplayTag ContextTag);
	
	/**
	 * @brief Request to show given context
	 * @param ContextTag Tag requested to be shown
	 */
	void ShowContext(FGameplayTag ContextTag);
	
	/**
	 * @brief Request to hide given context
	 * @param ContextTag Tag requested to be hidden
	 */
	void HideContext(FGameplayTag ContextTag);

	void SetUISubsystem(UArcUISubsystem* InUISubsystem) { UISubsystem = InUISubsystem; }

	UGameInstance* GetGameInstance() const;
	virtual UWorld* GetWorld() const override;

protected:
	virtual bool HandleOnContextAdded(FGameplayTag ContextTag, const TInstancedStruct<FArcUIContextPayload>& Payload) { return false; }
	virtual bool HandleOnContextRemoved(FGameplayTag ContextTag) { return false; }
	virtual bool HandleShowContext(FGameplayTag ContextTag) { return false; }
	virtual bool HandleHideContext(FGameplayTag ContextTag) { return false; }

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnContextAdded")
	void BP_HandleOnContextAdded(FGameplayTag ContextTag, const TInstancedStruct<FArcUIContextPayload>& Payload);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnContextRemoved")
	void BP_HandleOnContextRemoved(FGameplayTag ContextTag);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnContextShown")
	void BP_HandleShowContext(FGameplayTag ContextTag);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnContextHidden")
	void BP_HandleHideContext(FGameplayTag ContextTag);

	template <typename T>
	T* GetWidget() const
	{
		return Cast<T>(Widget);
	}
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Categories="ArcUI.Context"))
	FGameplayTagContainer ContextTags;

	UPROPERTY(Transient)
	TObjectPtr<UArcUISubsystem> UISubsystem{nullptr};

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> Widget{nullptr};

	UPROPERTY(Transient)
	TObjectPtr<AActor> ObservedActor{nullptr};
};


USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIPresenterInfo final : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(BlueprintBaseOnly, AllowAbstract=false))
	TSoftClassPtr<UArcUIPresenter> Presenter{nullptr};

	/** Should this presenter always be loaded */
	UPROPERTY(EditAnywhere)
	bool bKeepAlwaysLoaded{false};

	/** Should this presenter be loaded when a context is added */
	UPROPERTY(EditAnywhere, meta=(EditCondition="!bKeepAlwaysLoaded"))
	bool bLoadingTiedToContext{false};

	/** Which contexts can trigger the loading of the Presenter */
	UPROPERTY(EditAnywhere, meta=(Categories="ArcUI.Context", EditCondition="!bKeepAlwaysLoaded && bLoadingTiedToContext"))
	FGameplayTag ContextTag;
};
