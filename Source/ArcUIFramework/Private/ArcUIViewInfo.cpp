// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUIViewInfo.h"

// ArcUI
#include "ArcUIConditionSchema.h"

void FArcUIViewInfo::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);

	for (auto& Action : ContextActions)
	{
		Action.LoadingCondition.SetSchemaClass(UArcUIConditionSchema::StaticClass());
		Action.CreationCondition.SetSchemaClass(UArcUIConditionSchema::StaticClass());		
	}
}