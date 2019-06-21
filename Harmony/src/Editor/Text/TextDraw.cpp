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

TextDraw::TextDraw(ProjectItem *pProjItem) :
	IDraw(pProjItem),
	m_Text("", "+GuiPreview", this)
{
	m_Text.TextSet("The quick brown fox jumped over the lazy dog.");
}

TextDraw::~TextDraw()
{
}

/*virtual*/ void TextDraw::OnApplyJsonData(jsonxx::Value &valueRef) /*override*/
{
	texture_atlas_t *pFtglAtlas = static_cast<FontModel *>(m_pProjItem->GetModel())->GetFtglAtlas();
	unsigned char *pAtlasPixelData = static_cast<FontModel *>(m_pProjItem->GetModel())->GetAtlasPreviewPixelData();
	uint uiAtlasPixelDataSize = static_cast<FontModel *>(m_pProjItem->GetModel())->GetAtlasPreviewPixelDataSize();
	if(pFtglAtlas == nullptr || pAtlasPixelData == nullptr || Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer() == nullptr)
		return;

	if(pFtglAtlas->id == 0)
	{
		if(pFtglAtlas->id != HY_UNUSED_HANDLE)
			Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->DeleteTexture(pFtglAtlas->id);

		// Upload texture to gfx api
		pFtglAtlas->id = Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->AddTexture(HYTEXTURE_R8G8B8A8,
																										  HYTEXFILTER_BILINEAR,
																										  0,
																										  static_cast<uint32>(pFtglAtlas->width),
																										  static_cast<uint32>(pFtglAtlas->height),
																										  pAtlasPixelData,
																										  uiAtlasPixelDataSize,
																										  HYTEXTURE_R8G8B8A8);
	}

	m_Text.GuiOverrideData<HyText2dData>(valueRef, pFtglAtlas->id);
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
