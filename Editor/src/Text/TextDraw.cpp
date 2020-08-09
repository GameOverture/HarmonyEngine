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
#include "ProjectItemData.h"
#include "TextModel.h"
#include "Harmony.h"
#include "HarmonyWidget.h"

TextDraw::TextDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef),
	m_Text("", "+GuiPreview", this),
	m_hTexture(HY_UNUSED_HANDLE)
{
	m_Text.TextSet("The Quick Brown Fox Jumped Over The Lazy Dog!? 1234567890");
	m_Text.TextSetAlignment(HYALIGN_Center);
}

TextDraw::~TextDraw()
{
}

void TextDraw::SetTextState(uint uiStateIndex)
{
	m_Text.TextSetState(uiStateIndex);
}

/*virtual*/ void TextDraw::OnApplyJsonData(HyJsonObj itemDataObj) /*override*/
{
	uint uiAtlasPixelDataSize = 0;
	QSize atlasDimensions;
	unsigned char *pAtlasPixelData = static_cast<TextModel *>(m_pProjItem->GetModel())->GetFontManager().GetAtlasInfo(uiAtlasPixelDataSize, atlasDimensions);
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
																								  0,
																								  pAtlasPixelData,
																								  uiAtlasPixelDataSize,
																								  HYTEXTURE_R8G8B8A8);
	m_Text.GuiOverrideData<HyText2dData>(itemDataObj, m_hTexture);
	m_Text.TextSet("The Quick Brown Fox Jumped Over The Lazy Dog!? 1234567890");
}

/*virtual*/ void TextDraw::OnShow() /*override*/
{
	m_Text.SetVisible(true);
}

/*virtual*/ void TextDraw::OnHide() /*override*/
{
	SetVisible(false, true);
}

/*virtual*/ void TextDraw::OnResizeRenderer() /*override*/
{
}
