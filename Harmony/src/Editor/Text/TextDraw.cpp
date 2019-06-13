/**************************************************************************
*	TextDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TextDraw.h"

TextDraw::TextDraw(ProjectItem *pProjItem) :
	IDraw(pProjItem)
{
}

TextDraw::~TextDraw()
{
}

/*virtual*/ void TextDraw::OnShow() /*override*/
{
}

/*virtual*/ void TextDraw::OnHide() /*override*/
{
}

/*virtual*/ void TextDraw::OnResizeRenderer() /*override*/
{
}
