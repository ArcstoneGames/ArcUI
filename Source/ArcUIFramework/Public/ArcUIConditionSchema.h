// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE
#include "WorldConditionSchema.h"
// generated
#include "ArcUIConditionSchema.generated.h"


UCLASS()
class ARCUIFRAMEWORK_API UArcUIConditionSchema : public UWorldConditionSchema
{
	GENERATED_BODY()

public:
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;
};
