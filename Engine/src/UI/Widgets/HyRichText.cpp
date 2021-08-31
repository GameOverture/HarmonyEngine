/**************************************************************************
*	HyRichText.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyRichText.h"

HyRichText::HyRichText(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent)
{
}

/*virtual*/ HyRichText::~HyRichText()
{
}

/*virtual*/ glm::ivec2 HyRichText::GetSizeHint() /*override*/
{
	return glm::ivec2(0.0f, 0.0f);
}

/*virtual*/ glm::vec2 HyRichText::GetPosOffset() /*override*/
{
	return glm::vec2(0.0f, 0.0f);
}

/*virtual*/ void HyRichText::OnResize(int32 iNewWidth, int32 iNewHeight) /*override*/
{
}
