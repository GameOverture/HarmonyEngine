/**************************************************************************
 *	IApplication.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IApplication.h"

#include "FileIO/HyFileIO.h"

HyMemoryHeap	IApplication::sm_Mem;

IApplication::IApplication(HarmonyInit &initStruct) : m_Viewport(initStruct)
{
	m_Init = initStruct;
	CtorInit();

	HyFileIO::SetDataDir(m_Init.szDataDir);
}

IApplication::~IApplication()
{
}

void IApplication::CtorInit()
{
	if(m_Init.eDefaultCoordinateType == HYCOORD_Default)
		m_Init.eDefaultCoordinateType = HYCOORD_Pixel;

	HyAssert(m_Init.uiNumInputMappings >= 0, "HarmonyInit::uiNumInputMappings cannot be a negative value");
	m_pInputArray = new HyInputMapping[m_Init.uiNumInputMappings];
}

void * IApplication::operator new(tMEMSIZE size)
{
	return sm_Mem.Alloc(size);
}

void IApplication::operator delete(void *ptr)
{
	sm_Mem.Free(ptr);
}
