// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUITester.h"

#include <functional>

#include "ArcUISubsystem.h"
#include "ArcUILayout.h"
#include "ArcUIModel.h"
#include "CommonActivatableWidget.h"

void UArcUITester_Widget::UpdateWithModel() const
{
	if (bImplementsInterface && Model.IsValid())
	{
		if (auto* ModelReceiver = Cast<IArcUIModelReceiver>(Widget))
		{
			ModelReceiver->UpdateWithModel(Model);
		}
	}
}

void UArcUITester_Widget::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetPrevNode()->GetValue()->GetFName() == GET_MEMBER_NAME_CHECKED(UArcUITester_Widget, Model))
	{
		UpdateWithModel();
	}
}

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
#if WITH_EDITOR
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
#endif
}

void AArcUITester::AddExclusiveContext()
{
#if WITH_EDITOR
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
#endif
}

void AArcUITester::RemoveContext()
{
#if WITH_EDITOR
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
#endif
}

void AArcUITester::RebuildLayout()
{
	Layout.Layers.Empty();
	
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
					const FArcUITester_Layer TesterLayer{Tag, LayerWidget->GetName(), TesterWidget};
					
					Layout.Layers.Add(TesterLayer);
				}
			}
		}
	}
}
