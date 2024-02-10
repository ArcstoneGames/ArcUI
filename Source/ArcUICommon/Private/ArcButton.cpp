// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcButton.h"

// ArcUICommon
#include "ArcUICommonTypes.h"
// CommonUI
#include "CommonTextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcButton)


#if WITH_EDITOR
const FText UArcButton::GetPaletteCategory()
{
	return ArcUICommon::PaletteCategory;
}

void UArcButton::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (TXT_Label)
	{
		TXT_Label->SetText(LabelText);
	}
}
#endif

void UArcButton::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	TXT_Label->SetStyle(GetCurrentTextStyleClass());
}
