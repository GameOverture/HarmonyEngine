/**************************************************************************
 *	HySprite2dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Data/HySprite2dData.h"

#include "Renderer/IHyRenderer.h"
#include "Utilities/HyFileIO.h"


uint32 HySprite2dFrame::GetGfxApiHandle() const
{
	return pAtlasGroup ? pAtlasGroup->GetGfxApiHandle(uiATLAS_GROUP_TEXTURE_INDEX) : 0;
}

uint32 HySprite2dFrame::GetActualTextureIndex() const
{
	return pAtlasGroup ? pAtlasGroup->GetActualGfxApiTextureIndex(uiATLAS_GROUP_TEXTURE_INDEX) : 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HySprite2dData::HySprite2dData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAtlasContainer &atlasContainerRef) :	IHyData(HYTYPE_Sprite2d, sPath),
																																	m_pAnimStates(NULL),
																																	m_uiNumStates(0)
{
	jsonxx::Array spriteStateArray = dataValueRef.get<jsonxx::Array>();

	m_uiNumStates = static_cast<uint32>(spriteStateArray.size());
	m_pAnimStates = reinterpret_cast<AnimState *>(HY_NEW unsigned char[sizeof(AnimState) * m_uiNumStates]);
	AnimState *pAnimStateWriteLocation = m_pAnimStates;

	for(uint32 i = 0; i < m_uiNumStates; ++i, ++pAnimStateWriteLocation)
	{
		jsonxx::Object spriteStateObj = spriteStateArray.get<jsonxx::Object>(i);

		new (pAnimStateWriteLocation)AnimState(spriteStateObj.get<jsonxx::String>("name"),
			spriteStateObj.get<jsonxx::Boolean>("loop"),
			spriteStateObj.get<jsonxx::Boolean>("reverse"),
			spriteStateObj.get<jsonxx::Boolean>("bounce"),
			spriteStateObj.get<jsonxx::Array>("frames"),
			atlasContainerRef);
	}
}

/*virtual*/ HySprite2dData::~HySprite2dData(void)
{
	for(uint32 i = 0; i < m_uiNumStates; ++i)
		m_pAnimStates[i].~AnimState();

	unsigned char *pAnimStatesBuffer = reinterpret_cast<unsigned char *>(m_pAnimStates);
	delete[] pAnimStatesBuffer;
	pAnimStatesBuffer = NULL;
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

HySprite2dData::AnimState::AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HyAtlasContainer &atlasContainerRef) :	m_sNAME(sName),
																																									m_bLOOP(bLoop),
																																									m_bREVERSE(bReverse),
																																									m_bBOUNCE(bBounce),
																																									m_uiNUMFRAMES(frameArray.empty() ? 1 : static_cast<uint32>(frameArray.size()))	// Cannot have '0' frames
{
	m_pFrames = reinterpret_cast<HySprite2dFrame *>(HY_NEW unsigned char[sizeof(HySprite2dFrame) * m_uiNUMFRAMES]);
	HySprite2dFrame *pFrameWriteLocation = m_pFrames;

	for(uint32 i = 0; i < m_uiNUMFRAMES; ++i, ++pFrameWriteLocation)
	{
		HyAtlasGroup *pAtlasGroup = nullptr;
		uint32 uiAtlasGroupTextureIndex = 0;
		HyRectangle<float> rUVRect(0.0f, 0.0f, 0.0f, 0.0f);
		glm::ivec2 vOffset(0);
		float fDuration(0.0f);

		if(frameArray.empty() == false)
		{
			jsonxx::Object frameObj = frameArray.get<jsonxx::Object>(i);

			pAtlasGroup = atlasContainerRef.GetAtlasGroup(static_cast<uint32>(frameObj.get<jsonxx::Number>("atlasGroupId")));
			pAtlasGroup->GetUvRect(static_cast<uint32>(frameObj.get<jsonxx::Number>("checksum")), uiAtlasGroupTextureIndex, rUVRect);
			HySetVec(vOffset, static_cast<int32>(frameObj.get<jsonxx::Number>("offsetX")), static_cast<int32>(frameObj.get<jsonxx::Number>("offsetY")));
			fDuration = static_cast<float>(frameObj.get<jsonxx::Number>("duration"));
		}

		new (pFrameWriteLocation)HySprite2dFrame(pAtlasGroup,
												 uiAtlasGroupTextureIndex,
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

const HySprite2dFrame &HySprite2dData::AnimState::GetFrame(uint32 uiFrameIndex)
{
	return m_pFrames[uiFrameIndex];
}

/*virtual*/ void HySprite2dData::AppendRequiredAtlasIds(std::set<uint32> &requiredAtlasIdsOut)
{
	HyError("Not implemented");
}
