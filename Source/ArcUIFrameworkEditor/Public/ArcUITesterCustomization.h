// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"

struct FArcUITesterCustomization final : public IDetailCustomization
{
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FArcUITesterCustomization);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
