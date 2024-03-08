// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcButton.h"

// ArcUICommon
#include "ArcUICommonTypes.h"
// CommonUI
#include "CommonTextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcButton)

void UArcButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (TXT_Label)
	{
		TXT_Label->SetText(LabelText);
	}

	OnClickedEventHandle = OnClicked().AddLambda([this]() { OnArcClicked.Broadcast(this, Payload); });
}

void UArcButton::NativeDestruct()
{
	if (OnClickedEventHandle.IsValid())
	{
		OnClicked().Remove(OnClickedEventHandle);
		OnClickedEventHandle.Reset();
	}

	Super::NativeDestruct();
}

void UArcButton::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	if (TXT_Label)
	{
		TXT_Label->SetStyle(GetCurrentTextStyleClass());
	}
}

#if WITH_EDITOR
const FText UArcButton::GetPaletteCategory()
{
	return ArcUICommon::PaletteCategory;
}

void UArcButton::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();

	static FName LabelTextPropertyName(GET_MEMBER_NAME_CHECKED(UArcButton, LabelText));
	if (PropertyName == LabelTextPropertyName)
	{
		if (TXT_Label)
		{
			TXT_Label->SetText(LabelText);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif