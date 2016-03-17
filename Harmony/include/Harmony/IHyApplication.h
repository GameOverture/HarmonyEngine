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

	static HarmonyInit				sm_Init;
	static HyMemoryHeap				sm_Mem;

	vector<HyWindow>				m_vWindows;
	vector<IHyInputMap *>			m_vInputMaps;
	std::map<uint32, IHyShader *>	m_mapCustomShaders;

	virtual bool Initialize() = 0;
	virtual bool Update() = 0;
	virtual bool Shutdown() = 0;

public:
	IHyApplication(HarmonyInit &initStruct);
	~IHyApplication();

	static HyMemoryHeap &MemoryHeap()				{ return sm_Mem; }
	static HyCoordinateType DefaultCoordinateType()	{ HyAssert(sm_Init.eDefaultCoordinateType != HYCOORDTYPE_Default, "HyScene::DefaultCoordinateType() invoked before engine initialized"); return sm_Init.eDefaultCoordinateType; }
	static HyCoordinateUnit DefaultCoordinateUnit()	{ HyAssert(sm_Init.eDefaultCoordinateUnit != HYCOORDUNIT_Default, "HyScene::DefaultCoordinateUnit() invoked before engine initialized"); return sm_Init.eDefaultCoordinateUnit; }
	static float PixelsPerMeter()					{ return sm_Init.fPixelsPerMeter; }

	HyWindow &Window(uint32 uiIndex = 0);
	HyInputMapInterop &Input(uint32 uiIndex = 0);
	IHyShader &CustomShader(uint32 uiId);

	void *operator new(size_t uiSize);
	void operator delete (void *pPtr);
};

#endif /* __IHyApplication_h__ */
