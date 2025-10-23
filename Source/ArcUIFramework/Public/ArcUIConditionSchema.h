// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "WorldConditionSchema.h"
// generated
#include "ArcUIConditionSchema.generated.h"


UCLASS()
class ARCUIFRAMEWORK_API UArcUIConditionSchema : public UWorldConditionSchema
{
	GENERATED_BODY()

	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;
};
