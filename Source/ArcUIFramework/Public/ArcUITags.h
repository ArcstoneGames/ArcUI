// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "NativeGameplayTags.h"


namespace ArcUITags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArcUIRoot);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ViewRoot);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ContextRoot);

	namespace Layer
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(LayerRoot);

		namespace HUD
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Low);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(High);
		}

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Menu);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Modal);
	}
}
