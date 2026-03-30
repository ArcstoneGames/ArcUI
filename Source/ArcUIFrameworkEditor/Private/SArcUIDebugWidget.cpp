// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "SArcUIDebugWidget.h"
// ArcUIFramework
#include "ArcUIDebugSubsystem.h"
#include "ArcUILoader.h"
#include "ArcUIPresenter.h"
#include "ArcUISettings.h"
#include "ArcUISubsystem.h"
// UE
#include "Algo/Count.h"
#include "Editor.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"

namespace ArcUIDebug
{
	static constexpr float PanelWidth   = 600.f;
	static constexpr float PanelPadding = 6.f;
	static constexpr float ColWidth0    = 260.f;
	static constexpr float ColWidth1    = 80.f;
	static constexpr float ColWidth2    = 80.f;
	static constexpr float ColWidth3    = 80.f;
	static constexpr int32 MaxLogLines  = 50;

	static const FLinearColor BgColor          {0.04f, 0.04f, 0.06f, 0.95f};
	static const FLinearColor ColHeaderColor   {0.12f, 0.12f, 0.20f, 1.00f};
	static const FLinearColor AddedColor       {0.30f, 0.80f, 0.30f, 1.00f};
	static const FLinearColor RemovedColor     {0.90f, 0.35f, 0.35f, 1.00f};
	static const FLinearColor ActiveTagColor   {0.60f, 0.90f, 0.60f, 1.00f};
	static const FLinearColor LoadedColor      {0.30f, 0.80f, 0.30f, 1.00f};
	static const FLinearColor LoadedEmptyColor {0.90f, 0.55f, 0.10f, 1.00f}; // loaded but 0 instances
	static const FLinearColor UnloadedColor    {0.55f, 0.55f, 0.55f, 1.00f};
	static const FLinearColor AlwaysColor      {0.90f, 0.80f, 0.30f, 1.00f};

	static FSlateFontInfo MonoFont() { return FCoreStyle::GetDefaultFontStyle("Mono", 9); }
	static FSlateFontInfo BoldFont() { return FCoreStyle::GetDefaultFontStyle("Bold", 9); }
}

// ----------------------------------------------------------------------------
// Construct / Destruct
// ----------------------------------------------------------------------------

void SArcUIDebugWidget::Construct(const FArguments& InArgs)
{
	const TSharedRef<SArcUIDebugWidget> Self = StaticCastSharedRef<SArcUIDebugWidget>(AsShared());
	PIEStartedHandle = FEditorDelegates::PostPIEStarted.AddSP(Self, &SArcUIDebugWidget::OnPIEStarted);
	PIEEndedHandle   = FEditorDelegates::EndPIE.AddSP(Self, &SArcUIDebugWidget::OnPIEEnded);

	// Subscribe immediately if PIE is already running
	SubscribeToDebugSubsystem();
	RefreshData();

	ChildSlot
	[
		SNew(SBox).WidthOverride(ArcUIDebug::PanelWidth)
		[
			SNew(SBorder)
			.BorderBackgroundColor(ArcUIDebug::BgColor)
			.Padding(ArcUIDebug::PanelPadding)
			[
				SNew(SScrollBox)
				// --- CONTEXTS ---
				+ SScrollBox::Slot().Padding(0.f, 0.f, 0.f, 4.f)
				[
					SNew(SExpandableArea)
					.AreaTitle(FText::FromString(TEXT("CONTEXTS")))
					.InitiallyCollapsed(false)
					.BodyContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(SExpandableArea)
							.AreaTitle(FText::FromString(TEXT("Active")))
							.InitiallyCollapsed(false)
							.BodyContent()
							[
								SNew(SBox).MaxDesiredHeight(100.f)
								[
									SAssignNew(ActiveContextListView, SListView<TSharedPtr<FString>>)
									.ListItemsSource(&ActiveContextItems)
									.OnGenerateRow(this, &SArcUIDebugWidget::MakeActiveContextRow)
									.ScrollbarVisibility(EVisibility::Visible)
								]
							]
						]
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(SExpandableArea)
							.AreaTitle(FText::FromString(TEXT("Log")))
							.InitiallyCollapsed(false)
							.BodyContent()
							[
								SNew(SBox).MaxDesiredHeight(160.f)
								[
									SAssignNew(LogListView, SListView<TSharedPtr<FString>>)
									.ListItemsSource(&LogItems)
									.OnGenerateRow(this, &SArcUIDebugWidget::MakeLogRow)
									.ScrollbarVisibility(EVisibility::Visible)
								]
							]
						]
					]
				]
				// --- VIEWS ---
				+ SScrollBox::Slot().Padding(0.f, 0.f, 0.f, 4.f)
				[
					SNew(SExpandableArea)
					.AreaTitle(FText::FromString(TEXT("VIEWS")))
					.InitiallyCollapsed(false)
					.BodyContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight() [ MakeColumnHeader(TEXT("View Tag"), TEXT("Loaded"), TEXT("Instances"), TEXT("Always")) ]
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(SBox).MaxDesiredHeight(200.f)
							[
								SAssignNew(ViewListView, SListView<TSharedPtr<FArcUIDebugViewRow>>)
								.ListItemsSource(&ViewItems)
								.OnGenerateRow(this, &SArcUIDebugWidget::MakeViewRow)
								.ScrollbarVisibility(EVisibility::Visible)
							]
						]
					]
				]
				// --- PRESENTERS ---
				+ SScrollBox::Slot()
				[
					SNew(SExpandableArea)
					.AreaTitle(FText::FromString(TEXT("PRESENTERS")))
					.InitiallyCollapsed(false)
					.BodyContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight() [ MakeColumnHeader(TEXT("Class"), TEXT("Context Tags"), TEXT(""), TEXT("")) ]
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(SBox).MaxDesiredHeight(200.f)
							[
								SAssignNew(PresenterListView, SListView<TSharedPtr<FString>>)
								.ListItemsSource(&PresenterItems)
								.OnGenerateRow(this, &SArcUIDebugWidget::MakePresenterRow)
								.ScrollbarVisibility(EVisibility::Visible)
							]
						]
					]
				]
			]
		]
	];
}

SArcUIDebugWidget::~SArcUIDebugWidget()
{
	UnsubscribeFromDebugSubsystem();
	FEditorDelegates::PostPIEStarted.Remove(PIEStartedHandle);
	FEditorDelegates::EndPIE.Remove(PIEEndedHandle);
}

// ----------------------------------------------------------------------------
// PIE lifecycle
// ----------------------------------------------------------------------------

void SArcUIDebugWidget::OnPIEStarted(bool)
{
	SubscribeToDebugSubsystem();
	RefreshData();
}

void SArcUIDebugWidget::OnPIEEnded(bool)
{
	UnsubscribeFromDebugSubsystem();
	LastLogSize = 0;
	RefreshData();
}

void SArcUIDebugWidget::SubscribeToDebugSubsystem()
{
	UGameInstance* GameInstance = GEditor && GEditor->PlayWorld
		? GEditor->PlayWorld->GetGameInstance()
		: nullptr;

	if (auto* DebugSubsystem = GameInstance ? GameInstance->GetSubsystem<UArcUIDebugSubsystem>() : nullptr)
	{
		const TSharedRef<SArcUIDebugWidget> Self = StaticCastSharedRef<SArcUIDebugWidget>(AsShared());
		StateChangedHandle = DebugSubsystem->OnDebugStateChanged.AddSP(Self, &SArcUIDebugWidget::RefreshData);
	}
}

void SArcUIDebugWidget::UnsubscribeFromDebugSubsystem()
{
	UGameInstance* GameInstance = GEditor && GEditor->PlayWorld
		? GEditor->PlayWorld->GetGameInstance()
		: nullptr;

	if (auto* DebugSubsystem = GameInstance ? GameInstance->GetSubsystem<UArcUIDebugSubsystem>() : nullptr)
	{
		DebugSubsystem->OnDebugStateChanged.Remove(StateChangedHandle);
	}
	StateChangedHandle.Reset();
}

// ----------------------------------------------------------------------------
// RefreshData
// ----------------------------------------------------------------------------

void SArcUIDebugWidget::RefreshData()
{
	UGameInstance* GameInstance = GEditor && GEditor->PlayWorld
		? GEditor->PlayWorld->GetGameInstance()
		: nullptr;

	const auto* Subsystem      = GameInstance ? GameInstance->GetSubsystem<UArcUISubsystem>()      : nullptr;
	const auto* Loader         = GameInstance ? GameInstance->GetSubsystem<UArcUILoader>()          : nullptr;
	const auto* DebugSubsystem = GameInstance ? GameInstance->GetSubsystem<UArcUIDebugSubsystem>()  : nullptr;

	const bool bInPIE = Subsystem && Loader;

	// --- Context log (append-only) ---
	const TArray<FString>* ContextLog = DebugSubsystem ? &DebugSubsystem->GetContextLog() : nullptr;
	const int32 LogSize = ContextLog ? ContextLog->Num() : 0;

	if (LogSize != LastLogSize)
	{
		const int32 Start = FMath::Max(0, LogSize - ArcUIDebug::MaxLogLines);
		LogItems.Reset();
		for (int32 i = LogSize - 1; ContextLog && i >= Start; --i)
		{
			LogItems.Add(MakeShared<FString>((*ContextLog)[i]));
		}
		LastLogSize = LogSize;

		if (LogListView.IsValid())
		{
			LogListView->RequestListRefresh();
		}
	}

	// --- Active contexts ---
	ActiveContextItems.Reset();
	if (!bInPIE)
	{
		ActiveContextItems.Add(MakeShared<FString>(TEXT("(not in PIE)")));
	}
	else
	{
		const FGameplayTagContainer& ActiveTags = Subsystem->GetContextTags();
		if (ActiveTags.IsEmpty())
		{
			ActiveContextItems.Add(MakeShared<FString>(TEXT("(none)")));
		}
		else
		{
			for (const FGameplayTag& ActiveTag : ActiveTags)
			{
				const bool bHasPayload = Subsystem->HasPayload(ActiveTag);
				const FString Label = bHasPayload
					? FString::Printf(TEXT("%s  [payload]"), *ActiveTag.ToString())
					: ActiveTag.ToString();
				ActiveContextItems.Add(MakeShared<FString>(Label));
			}
		}
	}
	
	if (ActiveContextListView.IsValid())
	{
		ActiveContextListView->RequestListRefresh();
	}

	// --- Views ---
	ViewItems.Reset();
	if (bInPIE)
	{
		const auto* Settings = GetDefault<UArcUISettings>();
		const TMap<FGameplayTag, TSubclassOf<UUserWidget>>& LoadedClasses = Loader->GetManagedWidgetClasses();
		const TArray<FArcUIManagedWidget>& ManagedWidgets = Subsystem->GetManagedWidgets();

		TSet<FGameplayTag> KnownTags;
		TMap<FGameplayTag, bool> AlwaysLoadedMap;

		if (Settings)
		{
			for (const auto& ViewInfo : Settings->Views)
			{
				KnownTags.Add(ViewInfo.ViewTag);
				AlwaysLoadedMap.Add(ViewInfo.ViewTag, ViewInfo.bKeepAlwaysLoaded);
			}
		}
		
		for (const auto& [LoadedTag, Class] : LoadedClasses)
		{
			KnownTags.Add(LoadedTag);
		}
		
		for (const auto& Managed : ManagedWidgets)
		{
			KnownTags.Add(Managed.ViewTag);
		}

		for (const FGameplayTag& KnownTag : KnownTags)
		{
			auto Row           = MakeShared<FArcUIDebugViewRow>();
			Row->ViewTag       = KnownTag.ToString();
			Row->bLoaded       = LoadedClasses.Contains(KnownTag);
			Row->Instances     = Algo::CountIf(ManagedWidgets,
				[&KnownTag](const FArcUIManagedWidget& W) { return W.ViewTag == KnownTag; });
			Row->bAlwaysLoaded = AlwaysLoadedMap.FindRef(KnownTag);
			ViewItems.Add(MoveTemp(Row));
		}
	}
	
	if (ViewListView.IsValid())
	{
		ViewListView->RequestListRefresh();
	}

	// --- Presenters ---
	PresenterItems.Reset();
	if (!bInPIE)
	{
		PresenterItems.Add(MakeShared<FString>(TEXT("(not in PIE)")));
	}
	else
	{
		for (const auto& Presenter : Subsystem->GetPresenters())
		{
			if (!Presenter)
			{
				continue;
			}
			PresenterItems.Add(MakeShared<FString>(FString::Printf(TEXT("%-40s  %s"),
				*Presenter->GetClass()->GetName(),
				*Presenter->GetContextTags().ToStringSimple())));
		}
		if (PresenterItems.IsEmpty())
		{
			PresenterItems.Add(MakeShared<FString>(TEXT("(none)")));
		}
	}
	if (PresenterListView.IsValid())
	{
		PresenterListView->RequestListRefresh();
	}
}

// ----------------------------------------------------------------------------
// Row generators
// ----------------------------------------------------------------------------

TSharedRef<ITableRow> SArcUIDebugWidget::MakeLogRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& Owner) const
{
	const bool bAdded = Item->Contains(TEXT("[+]"));
	return SNew(STableRow<TSharedPtr<FString>>, Owner)
		[
			SNew(STextBlock).Text(FText::FromString(*Item))
			.Font(ArcUIDebug::MonoFont())
			.ColorAndOpacity(bAdded ? ArcUIDebug::AddedColor : ArcUIDebug::RemovedColor)
		];
}

TSharedRef<ITableRow> SArcUIDebugWidget::MakeActiveContextRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& Owner) const
{
	const bool bSpecial = *Item == TEXT("(none)") || *Item == TEXT("(not in PIE)");
	return SNew(STableRow<TSharedPtr<FString>>, Owner)
		[
			SNew(STextBlock).Text(FText::FromString(*Item))
			.Font(ArcUIDebug::MonoFont())
			.ColorAndOpacity(bSpecial ? ArcUIDebug::UnloadedColor : ArcUIDebug::ActiveTagColor)
		];
}

TSharedRef<ITableRow> SArcUIDebugWidget::MakeViewRow(TSharedPtr<FArcUIDebugViewRow> Item, const TSharedRef<STableViewBase>& Owner) const
{
	const FLinearColor RowColor = !Item->bLoaded
		? ArcUIDebug::UnloadedColor
		: (Item->Instances == 0 ? ArcUIDebug::LoadedEmptyColor : ArcUIDebug::LoadedColor);

	return SNew(STableRow<TSharedPtr<FArcUIDebugViewRow>>, Owner)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().MaxWidth(ArcUIDebug::ColWidth0)
			[
				SNew(STextBlock).Text(FText::FromString(Item->ViewTag))
				.Font(ArcUIDebug::MonoFont()).ColorAndOpacity(FLinearColor::White)
			]
			+ SHorizontalBox::Slot().MaxWidth(ArcUIDebug::ColWidth1)
			[
				SNew(STextBlock).Text(FText::FromString(Item->bLoaded ? TEXT("yes") : TEXT("no")))
				.Font(ArcUIDebug::MonoFont()).ColorAndOpacity(RowColor)
			]
			+ SHorizontalBox::Slot().MaxWidth(ArcUIDebug::ColWidth2)
			[
				SNew(STextBlock).Text(FText::FromString(FString::FromInt(Item->Instances)))
				.Font(ArcUIDebug::MonoFont()).ColorAndOpacity(FLinearColor::White)
			]
			+ SHorizontalBox::Slot().MaxWidth(ArcUIDebug::ColWidth3)
			[
				SNew(STextBlock).Text(FText::FromString(Item->bAlwaysLoaded ? TEXT("yes") : TEXT("no")))
				.Font(ArcUIDebug::MonoFont())
				.ColorAndOpacity(Item->bAlwaysLoaded ? ArcUIDebug::AlwaysColor : ArcUIDebug::UnloadedColor)
			]
		];
}

TSharedRef<ITableRow> SArcUIDebugWidget::MakePresenterRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& Owner) const
{
	const bool bSpecial = *Item == TEXT("(none)") || *Item == TEXT("(not in PIE)");
	return SNew(STableRow<TSharedPtr<FString>>, Owner)
		[
			SNew(STextBlock).Text(FText::FromString(*Item))
			.Font(ArcUIDebug::MonoFont())
			.ColorAndOpacity(bSpecial ? ArcUIDebug::UnloadedColor : FLinearColor::White)
		];
}

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------

TSharedRef<SWidget> SArcUIDebugWidget::MakeColumnHeader(
	const FString& Col0, const FString& Col1, const FString& Col2, const FString& Col3) const
{
	return SNew(SBorder)
		.BorderBackgroundColor(ArcUIDebug::ColHeaderColor)
		.Padding(2.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().MaxWidth(ArcUIDebug::ColWidth0)
			[ SNew(STextBlock).Text(FText::FromString(Col0)).Font(ArcUIDebug::BoldFont()).ColorAndOpacity(FLinearColor::White) ]
			+ SHorizontalBox::Slot().MaxWidth(ArcUIDebug::ColWidth1)
			[ SNew(STextBlock).Text(FText::FromString(Col1)).Font(ArcUIDebug::BoldFont()).ColorAndOpacity(FLinearColor::White) ]
			+ SHorizontalBox::Slot().MaxWidth(ArcUIDebug::ColWidth2)
			[ SNew(STextBlock).Text(FText::FromString(Col2)).Font(ArcUIDebug::BoldFont()).ColorAndOpacity(FLinearColor::White) ]
			+ SHorizontalBox::Slot().MaxWidth(ArcUIDebug::ColWidth3)
			[ SNew(STextBlock).Text(FText::FromString(Col3)).Font(ArcUIDebug::BoldFont()).ColorAndOpacity(FLinearColor::White) ]
		];
}
