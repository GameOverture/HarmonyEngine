/**************************************************************************
*	SpineDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "SpineDraw.h"
#include "ProjectItemData.h"
#include "SpineModel.h"
#include "Harmony.h"
#include "HarmonyWidget.h"

SpineDraw::SpineDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
	m_Spine.Init("", "+GuiPreview", this);
}

SpineDraw::~SpineDraw()
{
}

/*virtual*/ void SpineDraw::OnApplyJsonData(HyJsonObj itemDataObj) /*override*/
{
	//uint uiAtlasPixelDataSize = 0;
	//QSize atlasDimensions;
	//unsigned char *pAtlasPixelData = static_cast<SpineModel *>(m_pProjItem->GetModel())->GetFontManager().GetAtlasInfo(uiAtlasPixelDataSize, atlasDimensions);
	//if(pAtlasPixelData == nullptr || Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer() == nullptr)
	//	return;

	//if(m_hTexture != HY_UNUSED_HANDLE)
	//	Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->DeleteTexture(m_hTexture);

	//// Upload texture to gfx api
	//m_hTexture = Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->AddTexture(HyTextureInfo(HYTEXFILTER_BILINEAR, HYTEXTURE_Uncompressed, 4, 0),
	//	atlasDimensions.width(),
	//	atlasDimensions.height(),
	//	pAtlasPixelData,
	//	uiAtlasPixelDataSize,
	//	0);
}

/*virtual*/ void SpineDraw::OnShow() /*override*/
{
	SetVisible(true);
}
 
/*virtual*/ void SpineDraw::OnHide() /*override*/
{
	SetVisible(false, true);
}

/*virtual*/ void SpineDraw::OnResizeRenderer() /*override*/
{
}
