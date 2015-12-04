/**************************************************************************
 *	IHyApplication.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyApplication_h__
#define __IHyApplication_h__

#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"

#include "Memory/HyMemoryHeap.h"

class IHyApplication
{
	friend class HyEngine;

	HarmonyInit					m_Init;
	static HyMemoryHeap			sm_Mem;

	vector<HyWindow>			m_vWindows;
	vector<IHyInputMap *>		m_vInputMaps;

	virtual bool Initialize() = 0;
	virtual bool Update() = 0;
	virtual bool Shutdown() = 0;

public:
	IHyApplication(HarmonyInit &initStruct);
	~IHyApplication();

	static HyMemoryHeap &GetMemoryHeap()					{ return sm_Mem; }

	HyWindow &Window(uint32 uiIndex = 0);
	HyInputMapInterop &Input(uint32 uiIndex = 0);

	void *operator new(size_t size);
	void operator delete (void *ptr);
};

#endif /* __IHyApplication_h__ */
