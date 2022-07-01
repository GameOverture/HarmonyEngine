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
			QImage atlasImage(subAtlasRef.m_ImageFileInfo.absoluteFilePath());

			HyTextureHandle hNewTex = Harmony::GetWidget(&m_pProjItem->GetProject())->GetHarmonyRenderer()->AddTexture(
				HyTextureInfo(HYTEXFILTER_BILINEAR, HYTEXTURE_Uncompressed, 4, 0),
				atlasImage.width(),
				atlasImage.height(),
				atlasImage.bits(),
				atlasImage.byteCount(),
				0);

			rapidjson::Value value(rapidjson::kNumberType);
			value.SetUint(hNewTex);

			guiTexturesArray.PushBack(value, itemDataDocRef.GetAllocator());
		}

		itemDataObj.AddMember("guiTextures", guiTexturesArray, itemDataDocRef.GetAllocator());
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
