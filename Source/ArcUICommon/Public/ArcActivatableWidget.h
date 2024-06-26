// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// CommonUI
#include "CommonActivatableWidget.h"
// generated
#include "ArcActivatableWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ARCUICOMMON_API UArcActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
#pragma region UCommonActivatableWidget
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
#pragma endregion

protected:
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	
	UPROPERTY(EditDefaultsOnly, Category = Input, meta=(InlineEditConditionToggle))
	bool bOverrideInputConfig{false};
	
	UPROPERTY(EditDefaultsOnly, Category = Input, meta=(EditCondition="bOverrideInputConfig"))
	FUIInputConfig InputConfig;
};
