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

#include "Renderer/Viewport/HyWindow.h"

HarmonyInit IHyApplication::sm_Init;
HyMemoryHeap IHyApplication::sm_Mem;

IHyApplication::IHyApplication(HarmonyInit &initStruct)
{
	_crtBreakAlloc = 5609;

	sm_Init = initStruct;
	HyAssert(sm_Init.eDefaultCoordinateType != HYCOORDTYPE_Default, "HarmonyInit's actual 'eDefaultCoordinateType' cannot be 'HYCOORDTYPE_Default'");
	HyAssert(sm_Init.eDefaultCoordinateUnit != HYCOORDUNIT_Default, "HarmonyInit's actual 'eDefaultCoordinateUnit' cannot be 'HYCOORDUNIT_Default'");
	HyAssert(sm_Init.fPixelsPerMeter > 0.0f, "HarmonyInit's 'fPixelsPerMeter' cannot be <= 0.0f");
	
	for(uint32 i = 0; i < sm_Init.uiNumWindows; ++i)
	{
		m_vWindows.push_back(HY_NEW HyWindow());

		m_vWindows[i]->SetTitle(sm_Init.windowInfo[i].sName);
		m_vWindows[i]->SetResolution(sm_Init.windowInfo[i].vResolution);
		m_vWindows[i]->SetLocation(sm_Init.windowInfo[i].vLocation);
		m_vWindows[i]->SetType(sm_Init.windowInfo[i].eType);
	}

	for(uint32 i = 0; i < sm_Init.uiNumInputMappings; ++i)
		m_vInputMaps.push_back(HY_NEW HyInputMapInterop());
}

IHyApplication::~IHyApplication()
{
	m_vWindows.clear();
	m_vInputMaps.clear();
}

HyWindow &IHyApplication::Window(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < sm_Init.uiNumWindows, "IApplication::Viewport() took an invalid index: " << uiIndex);
	return *m_vWindows[uiIndex];
}

HyInputMapInterop &IHyApplication::Input(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < sm_Init.uiNumInputMappings, "IApplication::Input() took an invalid index: " << uiIndex);
	return *static_cast<HyInputMapInterop *>(m_vInputMaps[uiIndex]);
}

//void *IHyApplication::operator new(size_t uiSize)
//{
//	return sm_Mem.Alloc(uiSize);
//}
//
//void IHyApplication::operator delete(void *pPtr)
//{
//	sm_Mem.Free(pPtr);
//}
