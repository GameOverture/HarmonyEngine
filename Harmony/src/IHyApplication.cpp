/**************************************************************************
 *	IHyApplication.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IHyApplication.h"

HyMemoryHeap	IHyApplication::sm_Mem;

IHyApplication::IHyApplication(HarmonyInit &initStruct) :	m_Init(initStruct)
{
	HyAssert(m_Init.eDefaultCoordinateType != HYCOORD_Default, "HarmonyInit's actual 'eDefaultCoordinateType' cannot be 'HYCOORD_Default'");
	
	for(uint32 i = 0; i < m_Init.uiNumWindows; ++i)
	{
		m_vWindows.push_back(HyWindow());

		m_vWindows[i].SetTitle(m_Init.windowInfo[i].sName);
		m_vWindows[i].SetResolution(m_Init.windowInfo[i].vResolution);
		m_vWindows[i].SetLocation(m_Init.windowInfo[i].vLocation);
		m_vWindows[i].SetType(m_Init.windowInfo[i].eType);
	}

	for(uint32 i = 0; i < m_Init.uiNumInputMappings; ++i)
		m_vInputMaps.push_back(new HyInputMapInterop());
}

IHyApplication::~IHyApplication()
{
	for(uint32 i = 0; i < m_vInputMaps.size(); ++i)
		delete m_vInputMaps[i];

	m_vInputMaps.clear();
}

HyWindow &IHyApplication::Window(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < m_Init.uiNumWindows, "IApplication::Viewport() took an invalid index: " << uiIndex);
	return m_vWindows[uiIndex];
}

HyInputMapInterop &IHyApplication::Input(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < m_Init.uiNumInputMappings, "IApplication::Input() took an invalid index: " << uiIndex);
	return *static_cast<HyInputMapInterop *>(m_vInputMaps[uiIndex]);
}

void *IHyApplication::operator new(size_t size)
{
	return sm_Mem.Alloc(size);
}

void IHyApplication::operator delete(void *ptr)
{
	sm_Mem.Free(ptr);
}
