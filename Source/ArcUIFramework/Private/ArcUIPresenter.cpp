// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUIPresenter.h"

bool UArcUIPresenter::HasContextTag(FGameplayTag ContextTag) const
{
	return ContextTags.HasTag(ContextTag);
}
