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

/*virtual*/ void HySprite2dData::DoFileLoad(HyAtlasManager &atlasManagerRef)
{
	jsonxx::Object spriteObj;
	spriteObj.parse(IHyFileIO::ReadTextFile(m_sFILEPATH.c_str()));

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
											   atlasManagerRef);
	}
}

HySprite2dData::AnimState::AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HyAtlasManager &atlasManagerRef) :	sNAME(sName),
																																								bLOOP(bLoop),
																																								bREVERSE(bReverse),
																																								bBOUNCE(bBounce)
{
	//m_iNumFrames = m_iNumFrames
	//frameArray.get<jsonxx::Object>();

	//m_pAnimStates->m_iNumFrames = 1;
	//m_pAnimStates->m_pFrames = new AnimState::Frame[1];
	//m_pAnimStates->m_pFrames[0].m_fDur = 0.0f;
	//m_pAnimStates->m_pFrames[0].m_fRot = 0.0f;
	//m_pAnimStates->m_pFrames[0].m_iRectIndex = 0;
	//m_pAnimStates->m_pFrames[0].m_iTextureIndex = 0;
	//m_pAnimStates->m_pFrames[0].m_vOffset.x = m_pAnimStates->m_pFrames[0].m_vOffset.y = 0.0f;
	//m_pAnimStates->m_pFrames[0].m_vScale.x = m_pAnimStates->m_pFrames[0].m_vScale.y = 1.0f;
}

/*virtual*/ void HySprite2dData::OnGfxLoad(IHyRenderer &gfxApi)
{
}

/*virtual*/ void HySprite2dData::OnGfxRemove(IHyRenderer &gfxApi)
{
}
