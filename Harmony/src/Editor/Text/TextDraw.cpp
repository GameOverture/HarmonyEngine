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
#include "ProjectItem.h"
#include "TextModel.h"
#include "Harmony.h"
#include "HarmonyWidget.h"

TextDraw::TextDraw(ProjectItem *pProjItem) :
	IDraw(pProjItem),
	m_Text("", "+GuiPreview", this),
	m_hTexture(HY_UNUSED_HANDLE)
{
	m_Text.TextSet("The quick brown fox jumped over the lazy dog.");
}

TextDraw::~TextDraw()
{
}

/*virtual*/ void TextDraw::OnApplyJsonData(jsonxx::Value &valueRef) /*override*/
{
	uint uiAtlasPixelDataSize = 0;
	QSize atlasDimensions;
	unsigned char *pAtlasPixelData = static_cast<TextModel *>(m_pProjItem->GetModel())->GetFontManager().GenerateAtlas(uiAtlasPixelDataSize, atlasDimensions);
	if(pAtlasPixelData == nullptr || Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer() == nullptr)
		return;

	if(m_hTexture != HY_UNUSED_HANDLE)
		Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->DeleteTexture(m_hTexture);

	// Upload texture to gfx api
	m_hTexture = Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->AddTexture(HYTEXTURE_R8G8B8A8,
																								  HYTEXFILTER_BILINEAR,
																								  0,
																								  atlasDimensions.width(),
																								  atlasDimensions.height(),
																								  pAtlasPixelData,
																								  uiAtlasPixelDataSize,
																								  HYTEXTURE_R8G8B8A8);
	m_Text.GuiOverrideData<HyText2dData>(valueRef, m_hTexture);
	m_Text.TextSet("The quick brown fox jumped over the lazy dog.");
}

/*virtual*/ void TextDraw::OnShow() /*override*/
{
	m_Text.SetVisible(true);
}

/*virtual*/ void TextDraw::OnHide() /*override*/
{
	m_Text.SetVisible(false);
}

/*virtual*/ void TextDraw::OnResizeRenderer() /*override*/
{
}
