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
	m_hTexture(HY_UNUSED_HANDLE)
{
	m_Text.Init("", "+GuiPreview", this);
	SetPreviewText();
}

TextDraw::~TextDraw()
{
	if(m_hTexture != HY_UNUSED_HANDLE)
		Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->DeleteTexture(m_hTexture);
}

void TextDraw::SetTextState(uint uiStateIndex)
{
	m_Text.SetState(uiStateIndex);
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
	m_hTexture = Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->AddTexture(HyTextureInfo(HYTEXFILTER_BILINEAR, HYTEXTURE_Uncompressed, 4, 0),
																								  atlasDimensions.width(),
																								  atlasDimensions.height(),
																								  pAtlasPixelData,
																								  uiAtlasPixelDataSize,
																								  0);

	// Re-acquire latest FileDataPair because a newly generated preview texture may have been created above via GetAtlasInfo()
	FileDataPair itemFileData;
	m_pProjItem->GetLatestFileData(itemFileData);
	QByteArray src = JsonValueToSrc(itemFileData.m_Data);

	HyJsonDoc itemDataDoc;
	if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
		HyGuiLog("TextDraw::OnApplyJsonData failed to parse", LOGTYPE_Error);

	#undef GetObject
	m_Text.GuiOverrideData<HyTextData>(itemDataDoc.GetObject());
	m_Text.GetShaderUniforms().SetTexHandle(0, m_hTexture);
	SetPreviewText();
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

void TextDraw::SetPreviewText()
{
	m_Text.SetText(std::stringstream() <<
		"The Quick Brown Fox Jumped Over The Lazy Dog!?\n" <<
		"abcdefghijklmnopqrstuvwxyz\n" <<
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ\n" <<
		"1234567890 !$#%&()+,.");// /\\[]_{}|:;<>?@");

	// "ALL" SYMBOLS: ! \" # $ % & ' ( ) * + , - . / \\ [ ] ^ _ ` { | } ~ : ; < = > ? @
	// BAD SYMBOLS: ' * - " ^ ` ~ =
	
	m_Text.SetTextAlignment(HYALIGN_Center);
}
