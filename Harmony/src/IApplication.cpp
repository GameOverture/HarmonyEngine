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

IApplication::IApplication(HarmonyInit &initStruct) :	m_Init(initStruct)
{
	if(m_Init.eDefaultCoordinateType == HYCOORD_Default)
		m_Init.eDefaultCoordinateType = HYCOORD_Pixel;

	m_pViewports = new HyViewport(m_Init);
	//m_pInputArray = new HyInputMapping[m_Init.uiNumInputMappings];

	HyFileIO::SetDataDir(m_Init.szDataDir);
}

IApplication::~IApplication()
{
}

void * IApplication::operator new(tMEMSIZE size)
{
	return sm_Mem.Alloc(size);
}

void IApplication::operator delete(void *ptr)
{
	sm_Mem.Free(ptr);
}
