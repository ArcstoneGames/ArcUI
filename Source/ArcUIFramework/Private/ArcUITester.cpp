// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUITester.h"

#include <functional>

#include "ArcUISubsystem.h"
#include "ArcUILayout.h"
#include "ArcUIModel.h"
#include "CommonActivatableWidget.h"

void UArcUITester_Widget::UpdateWithModel() const
{
	if (bImplementsInterface && Model.Model.IsValid())
	{
		if (auto* ModelReceiver = Cast<IArcUIModelReceiver>(Widget))
		{
			ModelReceiver->UpdateWithModel(Model.Model);
		}
	}
}

#if WITH_EDITORONLY_DATA

AArcUITester::AArcUITester()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AArcUITester::BeginPlay()
{
	Super::BeginPlay();

	RebuildLayout();
}

void AArcUITester::AddContext()
{
	if (GetWorld()->WorldType == EWorldType::PIE)
	{
		if (auto* UISubsystem = GetGameInstance()->GetSubsystem<UArcUISubsystem>())
		{
			if (Context.IsValid())
			{
				UISubsystem->AddContextWithPayload(Context, ContextPayload);

				RebuildLayout();
			}
		}
	}
}

void AArcUITester::AddExclusiveContext()
{
	if (GetWorld()->WorldType == EWorldType::PIE)
	{
		if (auto* UISubsystem = GetGameInstance()->GetSubsystem<UArcUISubsystem>())
		{
			if (Context.IsValid())
			{
				UISubsystem->AddExclusiveContextWithPayload(Context, ContextPayload);

				RebuildLayout();
			}
		}
	}
}

void AArcUITester::RemoveContext()
{
	if (GetWorld()->WorldType == EWorldType::PIE)
	{
		if (auto* UISubsystem = GetGameInstance()->GetSubsystem<UArcUISubsystem>())
		{
			if (Context.IsValid())
			{
				UISubsystem->RemoveContext(Context);

				RebuildLayout();
			}
		}
	}
}

void AArcUITester::RebuildLayout()
{
	UILayers.Empty();
	
	if (const auto* UISubsystem = GetGameInstance()->GetSubsystem<UArcUISubsystem>())
	{
		if (const auto* UILayout = UISubsystem->GetLayout())
		{
			for (const auto Tag : UILayout->GetLayerTags())
			{
				if (const auto* LayerWidget = UILayout->GetLayer(Tag))
				{
					std::function<UArcUITester_Widget*(const UUserWidget*)> MakeWidget;
					MakeWidget = [&](const UUserWidget* InWidget)
					{
						auto* TesterWidget = NewObject<UArcUITester_Widget>(this);
						if (InWidget)
						{
							TesterWidget->Widget = InWidget;
							TesterWidget->Name = InWidget->GetName();
							TesterWidget->bImplementsInterface = InWidget->Implements<UArcUIModelReceiver>();
							TesterWidget->Model.OnModelPushRequest.BindUObject(TesterWidget, &UArcUITester_Widget::UpdateWithModel);
							InWidget->WidgetTree->ForEachWidget([&](UWidget* Widget)
							{
								if (const auto* UserWidget = Cast<UUserWidget>(Widget))
								{
									TesterWidget->Children.Add(MakeWidget(UserWidget));
								}
							});
						}
						return TesterWidget;
					};
					auto* TesterWidget = MakeWidget(LayerWidget->GetActiveWidget());
					const FArcUITester_Layer TesterLayer{Tag, TesterWidget};
					
					UILayers.Add(TesterLayer);
				}
			}
		}
	}
}
#endif