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
	SpineModel *pSpineModel = static_cast<SpineModel *>(m_pProjItem->GetModel());
	if(pSpineModel->IsUsingTempFiles())
	{
		//for(auto hTex : m_hTextureList)
		//{
		//	if(hTex != HY_UNUSED_HANDLE)
		//		Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->DeleteTexture(hTex);
		//}
		//m_hTextureList.clear();

		//const QList<SpineSubAtlas> &subAtlasList = pSpineModel->GetSubAtlasList();
		//for(const SpineSubAtlas &subAtlas : subAtlasList)
		//{
		//	uint uiAtlasPixelDataSize = 0;
		//	QSize atlasDimensions;
		//	unsigned char *pAtlasPixelData = static_cast<SpineModel *>(m_pProjItem->GetModel())->GetFontManager().GetAtlasInfo(uiAtlasPixelDataSize, atlasDimensions);
		//	if(pAtlasPixelData == nullptr || Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer() == nullptr)
		//		return;

		//	HyTextureHandle hNewTex = Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->AddTexture(HyTextureInfo(HYTEXFILTER_BILINEAR, HYTEXTURE_Uncompressed, 4, 0),
		//		atlasDimensions.width(),
		//		atlasDimensions.height(),
		//		pAtlasPixelData,
		//		uiAtlasPixelDataSize,
		//		0);

		//	m_hTextureList.push_back(hNewTex);
		//}

		//m_Spine.GuiOverrideTextures(m_hTextureList);
	}


	m_Spine.GuiOverrideData<HySpineData>(itemDataObj);
	m_Spine.Load();
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
