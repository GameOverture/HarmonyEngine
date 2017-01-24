///**************************************************************************
// *	HyInput.cpp
// *	
// *	Harmony Engine
// *	Copyright (c) 2013 Jason Knobler
// *
// *	The zlib License (zlib)
// *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
// *************************************************************************/
#include "Afx/HyInteropAfx.h"

#include "Input/IHyInput.h"
#include "Input/IHyInputMap.h"


IHyInput::IHyInput(uint32 uiNumInputMappings) : m_uiNUM_INPUT_MAPS(uiNumInputMappings)
{
	m_pInputMaps = reinterpret_cast<HyInputMapInterop *>(HY_NEW unsigned char[sizeof(HyInputMapInterop) * m_uiNUM_INPUT_MAPS]);
	HyInputMapInterop *pWriteLoc = static_cast<HyInputMapInterop *>(m_pInputMaps);
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i, ++pWriteLoc)
		new (pWriteLoc)HyInputMapInterop(this);
}

/*virtual*/ IHyInput::~IHyInput()
{
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		static_cast<HyInputMapInterop *>(m_pInputMaps)[i].~HyInputMapInterop();

	unsigned char *pMemBuffer = reinterpret_cast<unsigned char *>(m_pInputMaps);
	delete[] pMemBuffer;
}

IHyInputMap *IHyInput::GetInputMapArray()
{
	return m_pInputMaps;
}
