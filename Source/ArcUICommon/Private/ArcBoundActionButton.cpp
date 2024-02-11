// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcBoundActionButton.h"

// ArcUICommon
#include "ArcUICommonTypes.h"
// CommonUI
#include "CommonInputSubsystem.h"
#include "CommonTextBlock.h"

#if WITH_EDITOR
const FText UArcBoundActionButton::GetPaletteCategory()
{
	return ArcUICommon::PaletteCategory;
}

void UArcBoundActionButton::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (Text_ActionName)
	{
		Text_ActionName->SetText(LabelText);
	}
}
#endif

void UArcBoundActionButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (Text_ActionName)
	{
		Text_ActionName->SetText(LabelText);
	}
}

void UArcBoundActionButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto* InputSubsystem = GetInputSubsystem())
	{
		InputMethodChangedHandle = InputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::HandleInputMethodChanged);
		HandleInputMethodChanged(InputSubsystem->GetCurrentInputType());
	}
}

void UArcBoundActionButton::NativeDestruct()
{
	if (auto* InputSubsystem = GetInputSubsystem())
	{
		InputSubsystem->OnInputMethodChangedNative.Remove(InputMethodChangedHandle);
		InputMethodChangedHandle.Reset();
	}
	
	Super::NativeDestruct();
}

ECommonInputType UArcBoundActionButton::GetInputMethod() const
{
	if (const auto* InputSubsystem = GetInputSubsystem())
	{
		return InputSubsystem->GetCurrentInputType();
	}
	return ECommonInputType::MouseAndKeyboard;
}

void UArcBoundActionButton::HandleInputMethodChanged(ECommonInputType NewInputMethod)
{
	TSubclassOf<UCommonButtonStyle> NewStyle = nullptr;

	if (NewInputMethod == ECommonInputType::Gamepad)
	{
		NewStyle = GamepadStyle;
	}
	else if (NewInputMethod == ECommonInputType::MouseAndKeyboard)
	{
		NewStyle = KeyboardStyle;
	}

	if (NewStyle)
	{
		SetStyle(NewStyle);
	}

	BP_InputMethodChanged(NewInputMethod);
}
