// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "Kismet/BlueprintFunctionLibrary.h"
// generated
#include "ArcUILibrary.generated.h"

class APlayerController;
class ULocalPlayer;

UCLASS()
class ARCUIFRAMEWORK_API UArcUILibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static FName SuspendInputForPlayer(APlayerController* PlayerController, FName SuspendReason);

	static FName SuspendInputForPlayer(const ULocalPlayer* LocalPlayer, FName SuspendReason);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static void ResumeInputForPlayer(APlayerController* PlayerController, FName SuspendToken);

	static void ResumeInputForPlayer(const ULocalPlayer* LocalPlayer, FName SuspendToken);

private:
	static int32 InputSuspensions;
};
