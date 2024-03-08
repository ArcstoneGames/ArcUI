// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// CommonUI
#include "CommonButtonBase.h"
// UE
#include "InstancedStruct.h"
// generated
#include "ArcButton.generated.h"

class UCommonTextBlock;

/**
 * Base struct for button payloads
 */
USTRUCT(BlueprintType)
struct ARCUICOMMON_API FArcButtonPayload
{
	GENERATED_BODY()

	virtual ~FArcButtonPayload() = default;
};


UCLASS(Abstract, meta = (DisableNativeTick))
class ARCUICOMMON_API UArcButton final : public UCommonButtonBase
{
	GENERATED_BODY()

public:
#pragma region UCommonButtonBase
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnCurrentTextStyleChanged() override;
#pragma endregion

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable)
	void SetPayload(const TInstancedStruct<FArcButtonPayload>& InPayload) { Payload = InPayload; }

	UFUNCTION(BlueprintPure)
	const TInstancedStruct<FArcButtonPayload>& GetPayload() const { return Payload; }

	template <typename T>
	const T* GetPayloadPtr() const
	{
		return Payload.GetPtr<T>();
	}

	DECLARE_MULTICAST_DELEGATE_TwoParams(FArcButtonDelegate, const UArcButton*, const TInstancedStruct<FArcButtonPayload>&);
	FArcButtonDelegate OnArcClicked;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TInstancedStruct<FArcButtonPayload> Payload;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FText LabelText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> TXT_Label;

	FDelegateHandle OnClickedEventHandle;
};
