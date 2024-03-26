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

struct FArcUILayerCustomization final : public IPropertyTypeCustomization
{
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FArcUILayerCustomization);
	}

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
};

struct FArcUIViewPayloadWrapperCustomization final : public IPropertyTypeCustomization
{
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
		return MakeShareable(new FArcUIViewPayloadWrapperCustomization);
    }

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
};