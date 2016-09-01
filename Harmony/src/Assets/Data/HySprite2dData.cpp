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

HySprite2dData::HySprite2dData(const std::string &sPath) :	IHyData2d(HYINST_Spine2d, sPath)
{
}

HySprite2dData::~HySprite2dData(void)
{
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

HySprite2dData::AnimState::AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HySprite2dData &dataRef) :	sNAME(sName),
																																						bLOOP(bLoop),
																																						bREVERSE(bReverse),
																																						bBOUNCE(bBounce),
																																						uiNUMFRAMES(static_cast<uint32>(frameArray.size()))
{
	pFrames = reinterpret_cast<Frame *>(HY_NEW unsigned char[sizeof(Frame) * uiNUMFRAMES]);
	Frame *pFrameWriteLocation = pFrames;

	for(uint32 i = 0; i < uiNUMFRAMES; ++i)
	{
		jsonxx::Object frameObj = frameArray.get<jsonxx::Object>(i);

		HyAtlasGroup *pAtlasGroup = dataRef.RequestTexture(static_cast<uint32>(frameObj.get<jsonxx::Number>("atlasId")));
		uint32 uiTextureIndex = static_cast<uint32>(frameObj.get<jsonxx::Number>("textureId"));

		if(pAtlasGroup->ContainsTexture(uiTextureIndex) == false)
			HyError("HyTextures::RequestTexture() Atlas group (" << static_cast<uint32>(frameObj.get<jsonxx::Number>("atlasId")) << ") does not contain texture index: " << uiTextureIndex);

		new (pFrameWriteLocation)Frame(pAtlasGroup, 
									   uiTextureIndex,
									   static_cast<uint32>(frameObj.get<jsonxx::Number>("rectIndex")),
									   glm::vec2(static_cast<float>(frameObj.get<jsonxx::Number>("xOffset")), static_cast<float>(frameObj.get<jsonxx::Number>("yOffset"))),
									   static_cast<float>(frameObj.get<jsonxx::Number>("rotation")),
									   glm::vec2(static_cast<float>(frameObj.get<jsonxx::Number>("xScale")), static_cast<float>(frameObj.get<jsonxx::Number>("yScale"))),
									   static_cast<float>(frameObj.get<jsonxx::Number>("duration")));
	}
}
