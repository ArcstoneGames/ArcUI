// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

struct FArcUIDebugViewRow
{
	FString ViewTag;
	bool    bLoaded{false};
	int32   Instances{0};
	bool    bAlwaysLoaded{false};
};

/**
 * Dockable editor panel showing ArcUI runtime state during PIE:
 * context event log, view status, and registered presenters.
 * Refreshes on events — no polling.
 */
class SArcUIDebugWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArcUIDebugWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SArcUIDebugWidget() override;

private:
	void RefreshData();

	void OnPIEStarted(bool bIsSimulating);
	void OnPIEEnded(bool bIsSimulating);
	void SubscribeToDebugSubsystem();
	void UnsubscribeFromDebugSubsystem();

	TSharedRef<ITableRow> MakeLogRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& Owner) const;
	TSharedRef<ITableRow> MakeActiveContextRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& Owner) const;
	TSharedRef<ITableRow> MakeViewRow(TSharedPtr<FArcUIDebugViewRow> Item, const TSharedRef<STableViewBase>& Owner) const;
	TSharedRef<ITableRow> MakePresenterRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& Owner) const;

	TSharedRef<SWidget> MakeColumnHeader(const FString& Col0, const FString& Col1, const FString& Col2, const FString& Col3) const;

	int32 LastLogSize{0};

	TArray<TSharedPtr<FString>>            LogItems;
	TArray<TSharedPtr<FString>>            ActiveContextItems;
	TArray<TSharedPtr<FArcUIDebugViewRow>> ViewItems;
	TArray<TSharedPtr<FString>>            PresenterItems;

	TSharedPtr<SListView<TSharedPtr<FString>>>            LogListView;
	TSharedPtr<SListView<TSharedPtr<FString>>>            ActiveContextListView;
	TSharedPtr<SListView<TSharedPtr<FArcUIDebugViewRow>>> ViewListView;
	TSharedPtr<SListView<TSharedPtr<FString>>>            PresenterListView;

	FDelegateHandle PIEStartedHandle;
	FDelegateHandle PIEEndedHandle;
	FDelegateHandle StateChangedHandle;
};
