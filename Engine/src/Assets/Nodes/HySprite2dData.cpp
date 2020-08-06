/**************************************************************************
 *	HySprite2dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Renderer/IHyRenderer.h"

HyTextureHandle HySprite2dFrame::GetGfxApiHandle() const
{
	return pAtlas ? pAtlas->GetTextureHandle() : HY_UNUSED_HANDLE;
}

bool HySprite2dFrame::IsValid() const
{
	return pAtlas != nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HySprite2dData::HySprite2dData(const std::string &sPath, HyJsonObj &itemDataObjRef, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_pAnimStates(nullptr),
	m_uiNumStates(0)
{
	HyJsonArray spriteStateArray = itemDataObjRef["stateArray"].GetArray();

	m_uiNumStates = spriteStateArray.Size();
	m_pAnimStates = reinterpret_cast<AnimState *>(HY_NEW unsigned char[sizeof(AnimState) * m_uiNumStates]);
	AnimState *pAnimStateWriteLocation = m_pAnimStates;

	for(uint32 i = 0; i < m_uiNumStates; ++i, ++pAnimStateWriteLocation)
	{
		HyJsonObj spriteStateObj = spriteStateArray[i].GetObjectA();

		new (pAnimStateWriteLocation)AnimState(spriteStateObj["loop"].GetBool(),
											   spriteStateObj["reverse"].GetBool(),
											   spriteStateObj["bounce"].GetBool(),
											   spriteStateObj["duration"].GetFloat(),
											   m_RequiredAtlases,
											   spriteStateObj["frames"].GetArray(),
											   assetsRef);
	}
}

/*virtual*/ HySprite2dData::~HySprite2dData(void)
{
	for(uint32 i = 0; i < m_uiNumStates; ++i)
		m_pAnimStates[i].~AnimState();

	unsigned char *pAnimStatesBuffer = reinterpret_cast<unsigned char *>(m_pAnimStates);
	delete[] pAnimStatesBuffer;
	pAnimStatesBuffer = nullptr;
}

uint32 HySprite2dData::GetNumStates() const
{
	return m_uiNumStates;
}

const HySprite2dData::AnimState &HySprite2dData::GetState(uint32 uiAnimStateIndex) const
{
	return m_pAnimStates[uiAnimStateIndex];
}

const HySprite2dFrame &HySprite2dData::GetFrame(uint32 uiAnimStateIndex, uint32 uiFrameIndex) const
{
	return m_pAnimStates[uiAnimStateIndex].GetFrame(uiFrameIndex);
}

HySprite2dData::AnimState::AnimState(bool bLoop,
									 bool bReverse,
									 bool bBounce,
									 float fDuration,
									 HyFilesManifest &requiredAtlasIndicesRef,
									 HyJsonArray &frameArray,
									 HyAssets &assetsRef) :
	m_bLOOP(bLoop),
	m_bREVERSE(bReverse),
	m_bBOUNCE(bBounce),
	m_fDURATION(fDuration),
	m_uiNUMFRAMES(frameArray.Empty() ? 1 : frameArray.Size())	// Cannot have '0' frames
{
	m_pFrames = reinterpret_cast<HySprite2dFrame *>(HY_NEW unsigned char[sizeof(HySprite2dFrame) * m_uiNUMFRAMES]);
	HySprite2dFrame *pFrameWriteLocation = m_pFrames;

	for(uint32 i = 0; i < m_uiNUMFRAMES; ++i, ++pFrameWriteLocation)
	{
		HyFileAtlas *pAtlas = nullptr;
		HyRectangle<float> rUVRect(0.0f, 0.0f, 0.0f, 0.0f);
		glm::ivec2 vOffset(0);
		float fDuration(0.0f);

		if(frameArray.Empty() == false)
		{
			HyJsonObj frameObj = frameArray[i].GetObjectA();

			pAtlas = assetsRef.GetAtlas(frameObj["checksum"].GetUint(), rUVRect);
			requiredAtlasIndicesRef.Set(pAtlas->GetManifestIndex());

			HySetVec(vOffset, frameObj["offsetX"].GetInt(), frameObj["offsetY"].GetInt());
			fDuration = frameObj["duration"].GetFloat();
		}

		new (pFrameWriteLocation)HySprite2dFrame(pAtlas,
												 rUVRect.left, rUVRect.top, rUVRect.right, rUVRect.bottom,
												 vOffset,
												 fDuration);
	}
}

HySprite2dData::AnimState::~AnimState()
{
	for(uint32 i = 0; i < m_uiNUMFRAMES; ++i)
		m_pFrames[i].~HySprite2dFrame();

	unsigned char *pSpriteFramesBuffer = reinterpret_cast<unsigned char *>(m_pFrames);
	delete[] pSpriteFramesBuffer;
	pSpriteFramesBuffer = NULL;
}

const HySprite2dFrame &HySprite2dData::AnimState::GetFrame(uint32 uiFrameIndex) const
{
	return m_pFrames[uiFrameIndex];
}
