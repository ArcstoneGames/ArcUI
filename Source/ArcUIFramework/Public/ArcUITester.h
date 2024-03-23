// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "GameFramework/Info.h"
// generated
#include "ArcUITester.generated.h"

struct FArcUIModel;
class UCommonActivatableWidgetContainerBase;
struct FArcUIContextPayload;
struct FArcUITester_Widget;


UCLASS(BlueprintType, DefaultToInstanced)
class ARCUIFRAMEWORK_API UArcUITester_Widget : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta=(NoResetToDefault))
    FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Model")
	TInstancedStruct<FArcUIModel> Model;
	
	UFUNCTION(CallInEditor, Category="Model")
	void UpdateWithModel() const;
	
	UPROPERTY(VisibleAnywhere, Instanced, meta=(NoResetToDefault))
	TArray<TObjectPtr<UArcUITester_Widget>> Children;

	UPROPERTY(Transient)
	TObjectPtr<const UUserWidget> Widget{nullptr};

	UPROPERTY(Transient)
	bool bImplementsInterface{false};

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};


USTRUCT(BlueprintType)
struct FArcUITester_Layer
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta=(NoResetToDefault))
	FGameplayTag Tag;

	UPROPERTY(VisibleAnywhere, meta=(NoResetToDefault))
	FString Name;

	UPROPERTY(VisibleAnywhere, Instanced, meta=(NoResetToDefault))
	TObjectPtr<UArcUITester_Widget> Widget{nullptr};
};


USTRUCT(BlueprintType)
struct FArcUITester_Layout
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta=(NoResetToDefault))
	TArray<FArcUITester_Layer> Layers;
};


UCLASS(CollapseCategories, HideCategories=(Transform, Replication, Physics, Actor, Networking, levelInstance, Cooking))
class ARCUIFRAMEWORK_API AArcUITester : public AInfo
{
	GENERATED_BODY()

public:
	AArcUITester();
	
	virtual void BeginPlay() override;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category="Context", meta=(Categories="ArcUI.Context"))
	FGameplayTag Context;

	UPROPERTY(EditAnywhere, Category="Context")
	TInstancedStruct<FArcUIContextPayload> ContextPayload;

	void AddContext();
	void AddExclusiveContext();
	void RemoveContext();

	UPROPERTY(VisibleAnywhere, meta=(NoResetToDefault))
	FArcUITester_Layout Layout;
#endif;

private:
	void RebuildLayout();
};
