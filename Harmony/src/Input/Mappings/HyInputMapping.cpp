/**************************************************************************
 *	HyInputMapping.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Input/Mappings/HyInputMapping.h"
#include "Input/HyInput.h"

HyInputMapping::HyInputMapping() :	m_pHyInput(NULL),
									m_pInputMap(NULL)
{

}


HyInputMapping::~HyInputMapping(void)
{
	delete m_pInputMap;
}

void HyInputMapping::SetHyInputPtr(HyInput *pHyInput)
{
	m_pHyInput = pHyInput;
	m_pInputMap = new gainput::InputMap(m_pHyInput->GetManager());
}
