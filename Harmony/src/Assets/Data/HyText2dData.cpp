/**************************************************************************
 *	HyText2dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Data/HyText2dData.h"

#include "Renderer/IHyRenderer.h"

HyText2dData::HyText2dData(const std::string &sPath, int32 iShaderId) : IHy2dData(HYINST_Text2d, sPath, iShaderId)
{
}

HyText2dData::~HyText2dData(void)
{
}

/*virtual*/ void HyText2dData::DoFileLoad()
{
	std::string sFontFileContents;
	HyReadTextFile(GetPath().c_str(), sFontFileContents);

	jsonxx::Object textObject;
	textObject.parse(sFontFileContents);

	jsonxx::Array stateArray = textObject.get<jsonxx::Array>("stateArray");
	//for(int i = 0; i < 

	//m_uiNumStates = static_cast<uint32>(spriteStateArray.size());
	//m_pAnimStates = reinterpret_cast<AnimState *>(HY_NEW unsigned char[sizeof(AnimState) * m_uiNumStates]);
	//AnimState *pAnimStateWriteLocation = m_pAnimStates;

	//for(uint32 i = 0; i < m_uiNumStates; ++i, ++pAnimStateWriteLocation)
	//{
	//	jsonxx::Object spriteStateObj = spriteStateArray.get<jsonxx::Object>(i);

	//	new (pAnimStateWriteLocation)AnimState(spriteStateObj.get<jsonxx::String>("name"),
	//		spriteStateObj.get<jsonxx::Boolean>("loop"),
	//		spriteStateObj.get<jsonxx::Boolean>("reverse"),
	//		spriteStateObj.get<jsonxx::Boolean>("bounce"),
	//		spriteStateObj.get<jsonxx::Array>("frames"),
	//		*this);
	//}
}
