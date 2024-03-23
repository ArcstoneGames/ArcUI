// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUITesterCustomization.h"

#include "ArcUITester.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"


#define LOCTEXT_NAMESPACE "MaterialAnalyzer"

void FArcUITesterCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);
	auto* Object = CastChecked<AArcUITester>(Objects[0].Get());

	if (Object->GetWorld()->WorldType != EWorldType::PIE)
	{
		const TSharedRef<IPropertyHandle> LayoutProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AArcUITester, Layout));
		DetailBuilder.HideProperty(LayoutProperty);
		return;
	}
	
	IDetailCategoryBuilder& ContextCategoryBuilder = DetailBuilder.EditCategory("Context", LOCTEXT("ContextCategory", "Context"));
	ContextCategoryBuilder.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AArcUITester, Context)));
	ContextCategoryBuilder.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AArcUITester, ContextPayload)));	
	ContextCategoryBuilder.AddCustomRow(LOCTEXT("ContextActions", "ChooseContextAction"))
			.WholeRowContent()
			[
				SNew(SHorizontalBox)
					
				+ SHorizontalBox::Slot()
				.Padding(2, 0)
				.HAlign(HAlign_Fill)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Text(LOCTEXT("FunctionNewInputArg", "Add Context"))
					.OnClicked(FOnClicked::CreateLambda([Object]()
					{
						Object->AddContext();
						return FReply::Handled();
					}))
				]

				+ SHorizontalBox::Slot()
				.Padding(2, 0)
				.HAlign(HAlign_Fill)
				[
					SNew(SButton)					
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Text(LOCTEXT("FunctionNewInputArg", "Add Exclusive Context"))
					.OnClicked(FOnClicked::CreateLambda([Object]()
					{
						Object->AddExclusiveContext();
						return FReply::Handled();
					}))
				]

				+ SHorizontalBox::Slot()
				.Padding(2, 0)
				.HAlign(HAlign_Fill)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Text(LOCTEXT("FunctionNewInputArg", "Remove Context"))
					.OnClicked(FOnClicked::CreateLambda([Object]()
					{
						Object->RemoveContext();
						return FReply::Handled();
					}))
				]
			];
}

#undef LOCTEXT_NAMESPACE
