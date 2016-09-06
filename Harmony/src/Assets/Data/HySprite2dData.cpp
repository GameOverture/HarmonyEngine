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

HySprite2dData::HySprite2dData(const std::string &sPath) :	IHyData2d(HYINST_Spine2d, sPath),
															m_pAnimStates(NULL),
															m_uiNumStates(0)
{
}

/*virtual*/ HySprite2dData::~HySprite2dData(void)
{
	delete[] m_pAnimStates;
}

/*virtual*/ void HySprite2dData::DoFileLoad()
{
	jsonxx::Array spriteStateArray;
	spriteStateArray.parse(HyReadTextFile(GetPath().c_str()));

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

HySprite2dData::AnimState::AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HySprite2dData &dataRef) :	m_sNAME(sName),
																																						m_bLOOP(bLoop),
																																						m_bREVERSE(bReverse),
																																						m_bBOUNCE(bBounce),
																																						m_uiNUMFRAMES(static_cast<uint32>(frameArray.size()))
{
	m_pFrames = reinterpret_cast<Frame *>(HY_NEW unsigned char[sizeof(Frame) * m_uiNUMFRAMES]);
	Frame *pFrameWriteLocation = m_pFrames;

	for(uint32 i = 0; i < m_uiNUMFRAMES; ++i, ++pFrameWriteLocation)
	{
		jsonxx::Object frameObj = frameArray.get<jsonxx::Object>(i);

		HyAtlasGroup *pAtlasGroup = dataRef.RequestTexture(static_cast<uint32>(frameObj.get<jsonxx::Number>("atlasGroupId")));
		uint32 uiTextureIndex = static_cast<uint32>(frameObj.get<jsonxx::Number>("textureId"));

		if(pAtlasGroup->ContainsTexture(uiTextureIndex) == false)
			HyError("HyTextures::RequestTexture() Atlas group (" << static_cast<uint32>(frameObj.get<jsonxx::Number>("atlasId")) << ") does not contain texture index: " << uiTextureIndex);

		HyRectangle<float> rUVRect;
		pAtlasGroup->GetFrame(static_cast<uint32>(frameObj.get<jsonxx::Number>("checksum")), rUVRect);

		new (pFrameWriteLocation)Frame(pAtlasGroup,
									   uiTextureIndex,
									   rUVRect.left, rUVRect.top, rUVRect.right, rUVRect.bottom,
									   glm::vec2(static_cast<float>(frameObj.get<jsonxx::Number>("xOffset")), static_cast<float>(frameObj.get<jsonxx::Number>("yOffset"))),
									   rUVRect.iTag == 0 ? false : true,
									   static_cast<float>(frameObj.get<jsonxx::Number>("duration")));
	}
}

HySprite2dData::AnimState::~AnimState()
{
	delete [] m_pFrames;
}
