/**************************************************************************
 *	HySprite2dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FileIO/Data/HySprite2dData.h"

#include "FileIO/IHyFileIO.h"

#include "Renderer/IHyRenderer.h"

HySprite2dData::HySprite2dData(const std::string &sPath) :	IHyData(HYINST_Spine2d, sPath)
{
}

HySprite2dData::~HySprite2dData(void)
{
}

/*virtual*/ void HySprite2dData::DoFileLoad()
{
	jsonxx::Object spriteObj;
	spriteObj.parse(IHyFileIO::ReadTextFile(GetPath().c_str()));

	jsonxx::Array animStatesArray = spriteObj.get<jsonxx::Array>("animStates");

	m_uiNumStates = animStatesArray.size();
	m_pAnimStates = reinterpret_cast<AnimState *>(new unsigned char[sizeof(AnimState) * m_uiNumStates]);
	AnimState *pAnimStateWriteLocation = m_pAnimStates;

	for(uint32 i = 0; i < m_uiNumStates; ++i)
	{
		jsonxx::Object animStateObj = animStatesArray.get<jsonxx::Object>(i);

		new (pAnimStateWriteLocation)AnimState(animStateObj.get<jsonxx::String>("name"),
											   animStateObj.get<jsonxx::Boolean>("loop"),
											   animStateObj.get<jsonxx::Boolean>("reverse"),
											   animStateObj.get<jsonxx::Boolean>("bounce"),
											   animStateObj.get<jsonxx::Array>("frames"),
											   *this);
	}
}

HySprite2dData::AnimState::AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HySprite2dData &dataRef) :	sNAME(sName),
																																						bLOOP(bLoop),
																																						bREVERSE(bReverse),
																																						bBOUNCE(bBounce),
																																						uiNUMFRAMES(frameArray.size())
{
	pFrames = reinterpret_cast<Frame *>(new unsigned char[sizeof(Frame) * uiNUMFRAMES]);
	Frame *pFrameWriteLocation = pFrames;

	for(uint32 i = 0; i < uiNUMFRAMES; ++i)
	{
		jsonxx::Object frameObj = frameArray.get<jsonxx::Object>(i);

		new (pFrameWriteLocation)Frame(dataRef.RequestTexture(static_cast<uint32>(frameObj.get<jsonxx::Number>("textureId"))),
									   static_cast<uint32>(frameObj.get<jsonxx::Number>("rectIndex")),
									   vec2(frameObj.get<jsonxx::Number>("xOffset"), frameObj.get<jsonxx::Number>("yOffset")),
									   frameObj.get<jsonxx::Number>("rotation"),
									   vec2(frameObj.get<jsonxx::Number>("xScale"), frameObj.get<jsonxx::Number>("yScale")),
									   frameObj.get<jsonxx::Number>("duration"));
	}
}

/*virtual*/ void HySprite2dData::OnGfxLoad(IHyRenderer &gfxApi)
{
}

/*virtual*/ void HySprite2dData::OnGfxRemove(IHyRenderer &gfxApi)
{
}
