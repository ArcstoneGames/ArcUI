// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUIConditionSchema.h"

// UE
#include "WorldConditionBase.h"

bool UArcUIConditionSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return InScriptStruct && InScriptStruct->IsChildOf(TBaseStructure<FWorldConditionBase>::Get());
}