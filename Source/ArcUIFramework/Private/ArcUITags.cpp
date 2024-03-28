// Copyright (C) Fabien Poupineau. All Rights Reserved.

#include "ArcUITags.h"

namespace ArcUITags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcUIRoot, "ArcUI", "Root tag for anything handled by the ArcUI plugin")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ViewRoot, "ArcUI.View", "Root tag for views handled by the ArcUI plugin")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ContextRoot, "ArcUI.Context", "Root tag for contexts handled by the ArcUI plugin")

	namespace Layer
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LayerRoot, "ArcUI.Layer", "Root tag for layers handled by the ArcUI plugin")

		namespace HUD
		{
			UE_DEFINE_GAMEPLAY_TAG(Low, "ArcUI.Layer.HUD.Low");
			UE_DEFINE_GAMEPLAY_TAG(High, "ArcUI.Layer.HUD.High");
		}

		UE_DEFINE_GAMEPLAY_TAG(Menu, "ArcUI.Layer.Menu");
		UE_DEFINE_GAMEPLAY_TAG(Modal, "ArcUI.Layer.Modal");
	}
}
