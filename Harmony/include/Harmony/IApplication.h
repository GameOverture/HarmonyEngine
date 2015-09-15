/**************************************************************************
 *	IApplication.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IGame_h__
#define __IGame_h__

#include "Afx/HyStdAfx.h"

#include "Memory/HyMemoryHeap.h"

#include "Creator/Viewport/HyViewport.h"
#include "Input/Mappings/HyInputMapping.h"

class IApplication
{
	friend class HyEngine;

	HarmonyInit				m_Init;
	static HyMemoryHeap		sm_Mem;

	HyViewport *			m_pViewports;
	HyInputMapping *		m_pInputMaps;

	virtual bool Initialize() = 0;
	virtual bool Update() = 0;
	virtual bool Shutdown() = 0;

public:
	IApplication(HarmonyInit &initStruct);
	~IApplication();

	static HyMemoryHeap &GetMemoryHeap()					{ return sm_Mem; }

	HyViewport &Viewport(uint32 uiIndex = 0);
	//HyInput &Input(uint32 uiIndex = 0);

	void *operator new(tMEMSIZE size);
	void operator delete (void *ptr);
};

#endif /* __IGame_h__ */
