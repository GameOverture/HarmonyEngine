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

#include "FileIO/HyFileIO.h"

HyMemoryHeap	IHyApplication::sm_Mem;

IHyApplication::IHyApplication(HarmonyInit &initStruct) :	m_Init(initStruct)
{
	if(m_Init.eDefaultCoordinateType == HYCOORD_Default)
		m_Init.eDefaultCoordinateType = HYCOORD_Pixel;

	for(uint32 i = 0; i < m_Init.uiNumWindows; ++i)
	{
		m_vViewports.push_back(HyViewport());

		m_vViewports[i].SetTitle(m_Init.windowInfo[i].sName);
		m_vViewports[i].SetResolution(m_Init.windowInfo[i].vResolution);
		m_vViewports[i].SetLocation(m_Init.windowInfo[i].vLocation);
		m_vViewports[i].SetType(m_Init.windowInfo[i].eType);
		m_vViewports[i].SetBitsPerPixel(m_Init.windowInfo[i].iBitsPerPixel);
	}
	//m_pInputArray = new HyInputMapping[m_Init.uiNumInputMappings];

	HyFileIO::SetDataDir(m_Init.szDataDir);
}

IHyApplication::~IHyApplication()
{
}

HyViewport &IHyApplication::Viewport(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < m_Init.uiNumWindows, "IApplication::Viewport() took an invalid index: " << uiIndex);
	return m_vViewports[uiIndex];
}

void * IHyApplication::operator new(tMEMSIZE size)
{
	return sm_Mem.Alloc(size);
}

void IHyApplication::operator delete(void *ptr)
{
	sm_Mem.Free(ptr);
}
