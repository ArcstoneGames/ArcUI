// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// CommonUI
#include "Input/CommonBoundActionButton.h"
// generated
#include "ArcBoundActionButton.generated.h"

enum class ECommonInputType : uint8;

UCLASS()
class ARCUICOMMON_API UArcBoundActionButton : public UCommonBoundActionButton
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_InputMethodChanged(ECommonInputType NewInputMethod);

	UFUNCTION(BlueprintCallable)
	ECommonInputType GetInputMethod() const;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> KeyboardStyle;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> GamepadStyle;
	
private:
	void HandleInputMethodChanged(ECommonInputType NewInputMethod);
	
	FDelegateHandle InputMethodChangedHandle;
};
