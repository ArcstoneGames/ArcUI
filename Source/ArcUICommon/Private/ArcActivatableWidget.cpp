// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcActivatableWidget.h"

#include "ArcUICommonTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcActivatableWidget)

TOptional<FUIInputConfig> UArcActivatableWidget::GetDesiredInputConfig() const
{
	if (bOverrideInputConfig)
	{
		return InputConfig;
	}
	return Super::GetDesiredInputConfig();
}

#if WITH_EDITOR
const FText UArcActivatableWidget::GetPaletteCategory()
{
	return ArcUICommon::PaletteCategory;
}
#endif
