/**************************************************************************
*	FontDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "FontDraw.h"
#include "FontWidget.h"
#include "MainWindow.h"
#include "Harmony.h"
#include "HarmonyWidget.h"

#define PreviewOffsetY -2500.0f

FontDraw::FontDraw(ProjectItem *pProjItem, IHyApplication &hyApp) : IDraw(pProjItem, hyApp),
																	m_pPreviewTextCamera(nullptr),
																	m_pAtlasPreviewTexQuad(nullptr),
																	m_DrawAtlasOutline(this),
																	m_DividerLine(this),
																	m_PreviewOriginHorz(this),
																	m_Text("", "+GuiPreview", this)
{
	m_pPreviewTextCamera = m_HyAppRef.Window().CreateCamera2d();
	m_pPreviewTextCamera->SetViewport(0.0f, 0.0f, 1.0f, 0.5f);
	m_pPreviewTextCamera->pos.Set(0.0f, -2500.0f);
	m_pPreviewTextCamera->SetEnabled(true);

	m_Text.pos.Set(0.0f, PreviewOffsetY);
	m_Text.TextSetAlignment(HYALIGN_Center);
	m_Text.TextSet("1234567890");
	
	m_pCamera->SetViewport(0.0f, 0.5f, 1.0f, 0.5f);

	m_DrawAtlasOutline.SetTint(1.0f, 0.0f, 0.0f);

	m_DividerLine.GetShape().SetAsBox(10000.0f, 10.0f);
	m_DividerLine.pos.Set(-5000.0f, hyApp.Window().GetFramebufferSize().y / 2 - 5.0f);
	m_DividerLine.SetTint(0.0f, 0.0f, 0.0f);
	m_DividerLine.UseWindowCoordinates();

	std::vector<glm::vec2> lineList(2, glm::vec2());

	lineList[0].x = -5000.0f;
	lineList[0].y = PreviewOffsetY;
	lineList[1].x = 5000.0f;
	lineList[1].y = PreviewOffsetY;
	m_PreviewOriginHorz.GetShape().SetAsLineChain(&lineList[0], static_cast<uint32>(lineList.size()));
	m_PreviewOriginHorz.SetLineThickness(2.0f);
	m_PreviewOriginHorz.SetTint(1.0f, 1.0f, 1.0f);
	m_PreviewOriginHorz.SetEnabled(false);
}

/*virtual*/ FontDraw::~FontDraw()
{
	delete m_pAtlasPreviewTexQuad;
	m_HyAppRef.Window().RemoveCamera(m_pPreviewTextCamera);
}

/*virtual*/ void FontDraw::OnApplyJsonData(jsonxx::Value &valueRef) /*override*/
{
	texture_atlas_t *pFtglAtlas = static_cast<FontModel *>(m_pProjItem->GetModel())->GetFtglAtlas();
	unsigned char *pAtlasPixelData = static_cast<FontModel *>(m_pProjItem->GetModel())->GetAtlasPreviewPixelData();
	uint uiAtlasPixelDataSize = static_cast<FontModel *>(m_pProjItem->GetModel())->GetAtlasPreviewPixelDataSize();
	if(pFtglAtlas == nullptr || pAtlasPixelData == nullptr)
		return;

	if(pFtglAtlas->id == 0)
	{
		if(m_pAtlasPreviewTexQuad && m_pAtlasPreviewTexQuad->GetTextureHandle() != HY_UNUSED_HANDLE)
			Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->DeleteTexture(m_pAtlasPreviewTexQuad->GetTextureHandle());
	
		// Upload texture to gfx api
		pFtglAtlas->id = Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->AddTexture(HYTEXTURE_R8G8B8A8, 0, static_cast<uint32>(pFtglAtlas->width), static_cast<uint32>(pFtglAtlas->height), pAtlasPixelData, uiAtlasPixelDataSize, HYTEXTURE_R8G8B8A8);
	
		// Create a (new) raw 'HyTexturedQuad2d' using a gfx api texture handle
		delete m_pAtlasPreviewTexQuad;
		m_pAtlasPreviewTexQuad = new HyTexturedQuad2d(pFtglAtlas->id, static_cast<uint32>(pFtglAtlas->width), static_cast<uint32>(pFtglAtlas->height), this);
		m_pAtlasPreviewTexQuad->Load();
		m_pAtlasPreviewTexQuad->SetTextureSource(0, 0, static_cast<uint32>(pFtglAtlas->width), static_cast<uint32>(pFtglAtlas->height));
		m_pAtlasPreviewTexQuad->SetEnabled(true);
	
		m_DrawAtlasOutline.SetWireframe(true);
		m_DrawAtlasOutline.GetShape().SetAsBox(static_cast<int>(pFtglAtlas->width), static_cast<int>(pFtglAtlas->height));
	
		// Calculate the proper zoom amount to fit the whole atlas width (plus some extra margin) in preview window
		float fExtraMargin = 25.0f;
		float fZoomAmt = ((m_HyAppRef.Window().GetFramebufferSize().x * 100.0f) / (pFtglAtlas->width + (fExtraMargin * 2.0f))) / 100.0f;
		m_pCamera->SetZoom(fZoomAmt);
	
		const b2AABB &aabbWorldViewBounds = m_pCamera->GetWorldViewBounds();
		m_pAtlasPreviewTexQuad->pos.Set(aabbWorldViewBounds.lowerBound.x + fExtraMargin, aabbWorldViewBounds.upperBound.y - pFtglAtlas->height - fExtraMargin);
		m_DrawAtlasOutline.pos.Set(m_pAtlasPreviewTexQuad->pos.Get());
	}
	
	m_Text.GuiOverrideData<HyText2dData>(valueRef, pFtglAtlas->id);
	m_Text.TextSet("1234567890");
}

/*virtual*/ void FontDraw::OnShow(IHyApplication &hyApp)
{
	if(m_pAtlasPreviewTexQuad)
		m_pAtlasPreviewTexQuad->SetEnabled(true);

	m_DrawAtlasOutline.SetEnabled(true);
	m_DividerLine.SetEnabled(true);
	m_PreviewOriginHorz.SetEnabled(true);
	
	m_pPreviewTextCamera->SetEnabled(true);

	m_Text.SetEnabled(true);
}

/*virtual*/ void FontDraw::OnHide(IHyApplication &hyApp)
{
	m_pPreviewTextCamera->SetEnabled(false);
	m_PreviewOriginHorz.SetEnabled(false);
	m_Text.SetEnabled(false);
	SetEnabled(false, true);
}

/*virtual*/ void FontDraw::OnResizeRenderer() /*override*/
{
	m_DividerLine.pos.Set(-5000.0f, m_HyAppRef.Window().GetFramebufferSize().y / 2 - 5.0f);
}

/*virtual*/ void FontDraw::OnUpdate() /*override*/
{
}
