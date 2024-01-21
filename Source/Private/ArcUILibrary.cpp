// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUILibrary.h"

// CommonUI
#include "CommonInputSubsystem.h"
// generated
#include UE_INLINE_GENERATED_CPP_BY_NAME(ArcUILibrary)

int32 UArcUILibrary::InputSuspensions = 0;

FName UArcUILibrary::SuspendInputForPlayer(APlayerController* PlayerController, FName SuspendReason)
{
	return SuspendInputForPlayer(PlayerController ? PlayerController->GetLocalPlayer() : nullptr, SuspendReason);
}

FName UArcUILibrary::SuspendInputForPlayer(const ULocalPlayer* LocalPlayer, FName SuspendReason)
{
	if (auto* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer))
	{
		InputSuspensions++;
		FName SuspendToken = SuspendReason;
		SuspendToken.SetNumber(InputSuspensions);

		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, true);

		return SuspendToken;
	}

	return NAME_None;
}

void UArcUILibrary::ResumeInputForPlayer(APlayerController* PlayerController, FName SuspendToken)
{
	ResumeInputForPlayer(PlayerController ? PlayerController->GetLocalPlayer() : nullptr, SuspendToken);
}

void UArcUILibrary::ResumeInputForPlayer(const ULocalPlayer* LocalPlayer, FName SuspendToken)
{
	if (SuspendToken == NAME_None)
	{
		return;
	}

	if (auto* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer))
	{
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, false);
	}
}
