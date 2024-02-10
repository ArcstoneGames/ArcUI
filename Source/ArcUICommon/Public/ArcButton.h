// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// CommonUI
#include "CommonButtonBase.h"
// generated
#include "ArcButton.generated.h"

class UCommonTextBlock;

UCLASS(Abstract, meta = (DisableNativeTick))
class ARCUICOMMON_API UArcButton final : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnCurrentTextStyleChanged() override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditInstanceOnly)
	FText LabelText;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* TXT_Label;
};
