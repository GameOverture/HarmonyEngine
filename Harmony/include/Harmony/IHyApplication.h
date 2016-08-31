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

#ifdef HY_PLATFORM_GUI
protected:
#endif
	static HarmonyInit				sm_Init;

	vector<HyWindow *>				m_vWindows;
	vector<IHyInputMap *>			m_vInputMaps;

	virtual bool Initialize() = 0;
	virtual bool Update() = 0;
	virtual void Shutdown() = 0;

public:
	IHyApplication(HarmonyInit &initStruct);
	virtual ~IHyApplication();

	static HyCoordinateType DefaultCoordinateType()	{ HyAssert(sm_Init.eDefaultCoordinateType != HYCOORDTYPE_Default, "HyScene::DefaultCoordinateType() invoked before engine initialized"); return sm_Init.eDefaultCoordinateType; }
	static HyCoordinateUnit DefaultCoordinateUnit()	{ HyAssert(sm_Init.eDefaultCoordinateUnit != HYCOORDUNIT_Default, "HyScene::DefaultCoordinateUnit() invoked before engine initialized"); return sm_Init.eDefaultCoordinateUnit; }
	static float PixelsPerMeter()					{ return sm_Init.fPixelsPerMeter; }

	HyWindow &Window(uint32 uiIndex = 0);
	HyInputMapInterop &Input(uint32 uiIndex = 0);
};

#endif /* __IHyApplication_h__ */
