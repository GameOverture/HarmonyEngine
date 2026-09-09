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

SpineDraw::SpineDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
	m_Spine.Init("", "+GuiPreview", this);

}

SpineDraw::~SpineDraw()
{
}

/*virtual*/ void SpineDraw::OnApplyJsonData(HyJsonDoc &itemDataDocRef) /*override*/
{
#undef GetObject
	HyJsonObj itemDataObj = itemDataDocRef.GetObject();

	SpineModel *pSpineModel = static_cast<SpineModel *>(m_pProjItem->GetModel());
	if(pSpineModel->IsUsingTempFiles())
	{
		HyJsonArray atlasesArray = itemDataObj["atlases"].GetArray();
		const QList<SpineSubAtlas> &subAtlasList = pSpineModel->GetSubAtlasList();

		rapidjson::Value guiTexturesArray(rapidjson::kArrayType);
		for(const auto &subAtlasRef : subAtlasList)
		{
			HyImageInfo loadHints;
			uint32 uiPixelDataSize;
			loadHints.SetNumChannels(4);
			
			uint8 *pPixelData = HyIO::ReadImage(subAtlasRef.m_ImageFileInfo.absoluteFilePath().toStdString().c_str(), loadHints, uiPixelDataSize);
			HyRendererInterop *pRenderer = Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer();
			HyTextureHandle hNewTex = pRenderer->AddTexture(loadHints, HyTextureIn(), pPixelData, uiPixelDataSize);
			HyIO::DeleteImage(pPixelData);

			rapidjson::Value value(rapidjson::kNumberType);
			value.SetUint(hNewTex);
			guiTexturesArray.PushBack(value, itemDataDocRef.GetAllocator());
		}

		itemDataObj.AddMember("guiTextures", guiTexturesArray, itemDataDocRef.GetAllocator());
	}

	m_Spine.GuiOverrideData<HySpineData>(itemDataObj);
	m_Spine.Load();
}

/*virtual*/ void SpineDraw::OnResizeRenderer() /*override*/
{
}

void SpineDraw::SetCrossFadePreview(SpineCrossFade *pCrossFade)
{
#ifdef HY_USE_SPINE
	m_Spine.SetAnimation(0, m_Spine.GetAnim(pCrossFade->m_sAnimOne.toStdString()), false);
	m_Spine.AddAnimation(0, m_Spine.GetAnim(pCrossFade->m_sAnimTwo.toStdString()), false, 0.0f);
#endif
}
