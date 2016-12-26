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

// Client supplies these initialization parameters to the engine
struct HarmonyInit
{
	std::string				sGameName;
	std::string				sDataDir;
	uint32					uiNumWindows;
	HyWindowInfo			windowInfo[HY_MAXWINDOWS];
	HyCoordinateType		eDefaultCoordinateType;
	HyCoordinateUnit		eDefaultCoordinateUnit;
	float					fPixelsPerMeter;
	uint32					uiNumInputMappings;
	uint16					uiDebugPort;

	HarmonyInit();
	HarmonyInit(std::string sHyProjFilePath);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IHyApplication
{
	friend class HyEngine;

#ifdef HY_PLATFORM_GUI
protected:
#endif
	static HarmonyInit				sm_Init;

	std::vector<HyWindow *>			m_WindowList;
	std::vector<IHyInputMap *>		m_InputMapList;

	virtual bool Initialize() = 0;
	virtual bool Update() = 0;
	virtual void Shutdown() = 0;

public:
	IHyApplication(HarmonyInit &initStruct);
	virtual ~IHyApplication();

	static HyCoordinateType DefaultCoordinateType();
	static HyCoordinateUnit DefaultCoordinateUnit();
	static float PixelsPerMeter();

	HyWindow &Window(uint32 uiIndex = 0);
	HyInputMapInterop &Input(uint32 uiIndex = 0);
};

#endif /* __IHyApplication_h__ */
