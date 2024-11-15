// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "GameFramework/Info.h"
// generated
#include "ArcUITester.generated.h"

struct FArcUIViewPayload;
class UCommonActivatableWidgetContainerBase;
struct FArcUIContextPayload;
struct FArcUITester_Widget;


USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIViewPayloadWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TInstancedStruct<FArcUIViewPayload> Payload;

	DECLARE_DELEGATE(FArcUIOnPushViewPayload);
	FArcUIOnPushViewPayload OnPushViewPayload;
};


UCLASS(BlueprintType, NotBlueprintable, DefaultToInstanced)
class ARCUIFRAMEWORK_API UArcUITester_Widget : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta=(NoResetToDefault))
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Payload")
	FArcUIViewPayloadWrapper ViewPayload;
	
	void PushViewPayload() const;
	
	UPROPERTY(VisibleAnywhere, Instanced, meta=(NoResetToDefault))
	TArray<TObjectPtr<UArcUITester_Widget>> Children;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> Widget{nullptr};

	UPROPERTY(Transient)
	bool bImplementsInterface{false};
};


USTRUCT(BlueprintType)
struct FArcUITester_Layer
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta=(NoResetToDefault))
	FGameplayTag Tag;

	UPROPERTY(VisibleAnywhere, Instanced, meta=(ShowOnlyInnerProperties, NoResetToDefault))
	TObjectPtr<UArcUITester_Widget> Widget{nullptr};
};


UCLASS(CollapseCategories, HideCategories=(Transform, Replication, Physics, Actor, Networking, levelInstance, Cooking))
class ARCUIFRAMEWORK_API AArcUITester : public AInfo
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA

public:
	AArcUITester();
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Context", meta=(Categories="ArcUI.Context"))
	FGameplayTag Context;

	UPROPERTY(EditAnywhere, Category="Context")
	TInstancedStruct<FArcUIContextPayload> ContextPayload;

	void AddContext();
	void AddExclusiveContext();
	void RemoveContext();

	UPROPERTY(VisibleAnywhere, DisplayName="Layers", Category="Widgets", meta=(ShowOnlyInnerProperties, NoResetToDefault, TitleProperty="Tag.ToString()"))
	TArray<FArcUITester_Layer> UILayers;

private:
	void RebuildLayout();

#endif;
};
