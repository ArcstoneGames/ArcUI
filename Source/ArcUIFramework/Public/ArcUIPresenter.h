// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "GameplayTagContainer.h"
// generated
#include "ArcUIPresenter.generated.h"

class UArcUISubsystem;
class UUserWidget;
class AActor;

/**
 * Base class for (standalone) Presenters
 */
UCLASS(Abstract)
class ARCUIFRAMEWORK_API UArcUIPresenter : public UObject
{
	GENERATED_BODY()

public:
	void SetUISubsystem(UArcUISubsystem* InUISubsystem) { UISubsystem = InUISubsystem; }

	/**
	 * @brief Retrieve the Context tags for this presenter
	 * @return The context tags affecting the presenter's widgets display
	 */
	const FGameplayTagContainer& GetContextTags() const { return ContextTags; }

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
	virtual void OnContextAdded(FGameplayTag ContextTag) {}

	/**
	 * @brief Called when a context is removed
	 * @param ContextTag Tag being removed from the context
	 */
	virtual void OnContextRemoved(FGameplayTag ContextTag) {}

protected:
	UPROPERTY(Transient)
	TObjectPtr<UArcUISubsystem> UISubsystem{nullptr};

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> Widget{nullptr};

	UPROPERTY(Transient)
	TObjectPtr<AActor> ObservedActor{nullptr};

	FGameplayTagContainer ContextTags;
};
