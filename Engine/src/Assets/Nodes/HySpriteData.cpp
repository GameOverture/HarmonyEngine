/**************************************************************************
 *	HySpriteData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HySpriteData.h"
#include "Renderer/IHyRenderer.h"

HyTextureHandle HySpriteFrame::GetGfxApiHandle() const
{
	return pAtlas ? pAtlas->GetTextureHandle() : HY_UNUSED_HANDLE;
}

bool HySpriteFrame::IsAtlasValid() const
{
	return pAtlas != nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HySpriteData::HySpriteData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_pAnimStates(nullptr)
{
	HyJsonArray spriteStateArray = itemDataObj["stateArray"].GetArray();

	m_uiNumStates = spriteStateArray.Size();
	m_pAnimStates = reinterpret_cast<AnimState *>(HY_NEW unsigned char[sizeof(AnimState) * m_uiNumStates]);
	AnimState *pAnimStateWriteLocation = m_pAnimStates;

	for(uint32 i = 0; i < m_uiNumStates; ++i, ++pAnimStateWriteLocation)
	{
		HyJsonObj spriteStateObj = spriteStateArray[i].GetObject();

		new (pAnimStateWriteLocation)AnimState(spriteStateObj["loop"].GetBool(),
											   spriteStateObj["reverse"].GetBool(),
											   spriteStateObj["bounce"].GetBool(),
											   spriteStateObj["duration"].GetFloat(),
											   m_RequiredFiles[HYFILE_Atlas],
											   spriteStateObj["frames"].GetArray(),
											   assetsRef);
	}
}

/*virtual*/ HySpriteData::~HySpriteData(void)
{
	for(uint32 i = 0; i < m_uiNumStates; ++i)
		m_pAnimStates[i].~AnimState();

	unsigned char *pAnimStatesBuffer = reinterpret_cast<unsigned char *>(m_pAnimStates);
	delete[] pAnimStatesBuffer;
	pAnimStatesBuffer = nullptr;
}

const HySpriteData::AnimState &HySpriteData::GetState(uint32 uiAnimStateIndex) const
{
	return m_pAnimStates[uiAnimStateIndex];
}

const HySpriteFrame &HySpriteData::GetFrame(uint32 uiAnimStateIndex, uint32 uiFrameIndex) const
{
	return m_pAnimStates[uiAnimStateIndex].GetFrame(uiFrameIndex);
}

HySpriteData::AnimState::AnimState(bool bLoop,
									 bool bReverse,
									 bool bBounce,
									 float fDuration,
									 HyFilesManifest &requiredAtlasIndicesRef,
									 HyJsonArray frameArray,
									 HyAssets &assetsRef) :
	m_bLOOP(bLoop),
	m_bREVERSE(bReverse),
	m_bBOUNCE(bBounce),
	m_fDURATION(fDuration),
	m_uiNUMFRAMES(frameArray.Empty() ? 1 : frameArray.Size())	// Cannot have '0' frames
{
	m_pFrames = reinterpret_cast<HySpriteFrame *>(HY_NEW unsigned char[sizeof(HySpriteFrame) * m_uiNUMFRAMES]);
	HySpriteFrame *pFrameWriteLocation = m_pFrames;

	for(uint32 i = 0; i < m_uiNUMFRAMES; ++i, ++pFrameWriteLocation)
	{
		HyFileAtlas *pAtlas = nullptr;
		HyRectangle<float> rUVRect(0.0f, 0.0f, 0.0f, 0.0f);
		uint64 uiCropMask = 0;
		glm::ivec2 vOffset(0);
		float fDuration(0.0f);

		if(frameArray.Empty() == false)
		{
			HyJsonObj frameObj = frameArray[i].GetObject();

			pAtlas = assetsRef.GetAtlas(frameObj["checksum"].GetUint(), frameObj["bankId"].GetUint(), rUVRect, uiCropMask);
			requiredAtlasIndicesRef.Set(pAtlas->GetManifestIndex());

			HySetVec(vOffset, frameObj["offsetX"].GetInt(), frameObj["offsetY"].GetInt());
			fDuration = frameObj["duration"].GetFloat();
		}

		new (pFrameWriteLocation)HySpriteFrame(pAtlas,
											   rUVRect.left, rUVRect.top, rUVRect.right, rUVRect.bottom,
											   uiCropMask,
											   vOffset,
											   fDuration);
	}
}

HySpriteData::AnimState::~AnimState()
{
	for(uint32 i = 0; i < m_uiNUMFRAMES; ++i)
		m_pFrames[i].~HySpriteFrame();

	unsigned char *pSpriteFramesBuffer = reinterpret_cast<unsigned char *>(m_pFrames);
	delete[] pSpriteFramesBuffer;
	pSpriteFramesBuffer = NULL;
}

const HySpriteFrame &HySpriteData::AnimState::GetFrame(uint32 uiFrameIndex) const
{
	return m_pFrames[uiFrameIndex];
}
