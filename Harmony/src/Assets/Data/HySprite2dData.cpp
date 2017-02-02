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
	return pAtlasGroup->GetGfxApiHandle(uiATLAS_GROUP_TEXTURE_INDEX);
}

uint32 HySprite2dFrame::GetActualTextureIndex() const
{
	return pAtlasGroup->GetActualGfxApiTextureIndex(uiATLAS_GROUP_TEXTURE_INDEX);
}

HySprite2dData::HySprite2dData(const std::string &sPath, int32 iShaderId) : IHy2dData(HYTYPE_Sprite2d, sPath, iShaderId),
																			m_pAnimStates(NULL),
																			m_uiNumStates(0)
{
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

/*virtual*/ void HySprite2dData::DoFileLoad()
{
	std::string sSpriteFileContents;
	HyReadTextFile(GetPath().c_str(), sSpriteFileContents);

	jsonxx::Array spriteStateArray;
	spriteStateArray.parse(sSpriteFileContents);

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
											   *this);
	}
}

HySprite2dData::AnimState::AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HySprite2dData &dataRef) : m_sNAME(sName),
																																						m_bLOOP(bLoop),
																																						m_bREVERSE(bReverse),
																																						m_bBOUNCE(bBounce),
																																						m_uiNUMFRAMES(static_cast<uint32>(frameArray.size()))
{
	m_pFrames = reinterpret_cast<HySprite2dFrame *>(HY_NEW unsigned char[sizeof(HySprite2dFrame) * m_uiNUMFRAMES]);
	HySprite2dFrame *pFrameWriteLocation = m_pFrames;

	for(uint32 i = 0; i < m_uiNUMFRAMES; ++i, ++pFrameWriteLocation)
	{
		jsonxx::Object frameObj = frameArray.get<jsonxx::Object>(i);

		HyAtlasGroup *pAtlasGroup = dataRef.RequestTexture(static_cast<uint32>(frameObj.get<jsonxx::Number>("atlasGroupId")));

		uint32 uiAtlasGroupTextureIndex;
		HyRectangle<float> rUVRect;
		pAtlasGroup->GetUvRect(static_cast<uint32>(frameObj.get<jsonxx::Number>("checksum")), uiAtlasGroupTextureIndex, rUVRect);

		new (pFrameWriteLocation)HySprite2dFrame(pAtlasGroup,
												 uiAtlasGroupTextureIndex,
												 rUVRect.left, rUVRect.top, rUVRect.right, rUVRect.bottom,
												 glm::ivec2(static_cast<int32>(frameObj.get<jsonxx::Number>("offsetX")), static_cast<int32>(frameObj.get<jsonxx::Number>("offsetY"))),
												 static_cast<float>(frameObj.get<jsonxx::Number>("duration")));
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
