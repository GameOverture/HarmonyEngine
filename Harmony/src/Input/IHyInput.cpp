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
#include "Renderer/Components/HyWindow.h"

IHyInput::IHyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef) :	m_uiNUM_INPUT_MAPS(uiNumInputMappings),
																						m_WindowListRef(windowListRef),
																						m_uiMouseWindowIndex(0)
{
	IHyInputMap::sm_pInputManager = this;

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

glm::vec2 IHyInput::GetWorldMousePos()
{
	m_csMouse.Lock();
	glm::vec2 vRetVec = m_WindowListRef[m_uiMouseWindowIndex]->ConvertViewportCoordinateToWorldPos(m_ptLocalMousePos);
	m_csMouse.Unlock();

	return vRetVec;
}

bool IHyInput::IsMouseLeftDown()
{
	m_csMouse.Lock();
	bool bReturnVal = m_bMouseLeftDown;
	m_csMouse.Unlock();

	return bReturnVal;
}

bool IHyInput::IsMouseRightDown()
{
	m_csMouse.Lock();
	bool bReturnVal = m_bMouseRightDown;
	m_csMouse.Unlock();

	return bReturnVal;
}

void IHyInput::CsLock()
{
	m_csMouse.Lock();
}

void IHyInput::CsUnlock()
{
	m_csMouse.Unlock();
}
