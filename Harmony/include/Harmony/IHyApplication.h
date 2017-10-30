/**************************************************************************
 *	IHyApplication.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyApplication_h__
#define IHyApplication_h__

#include "Afx/HyInteropAfx.h"
#include "Memory/HyMemoryHeap.h"
#include "Input/HyInputMap.h"

// Client supplies these initialization parameters to the engine
struct HarmonyInit
{
	std::string				sProjectDir;

	std::string				sGameName;
	std::string				sDataDir;
	uint32					uiNumWindows;
	HyWindowInfo			windowInfo[HY_MAXWINDOWS];
	uint32					uiUpdateTickMs;
	float					fPixelsPerMeter;
	uint32					uiNumInputMappings;
	uint16					uiDebugPort;
	bool					bUseConsole;
	bool					bShowCursor;
	HyWindowInfo			consoleInfo;

	HarmonyInit();
	HarmonyInit(std::string sHyProjFileName);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IHyApplication
{
	friend class HyEngine;

#ifdef HY_PLATFORM_GUI
protected:
#endif
	HarmonyInit						m_Init;

	std::vector<HyWindow *>			m_WindowList;

	HyInputMap *					m_pInputMaps;
	HyConsoleInterop				m_Console;

	virtual bool Initialize() = 0;
	virtual bool Update() = 0;
	virtual void Shutdown() = 0;

public:
	IHyApplication(HarmonyInit &initStruct);
	virtual ~IHyApplication();

	std::string GameName() const;
	std::string DataDir() const;
	bool IsShowCursor() const;

	HyWindow &Window(uint32 uiIndex = 0);
	uint32 GetNumWindows();
	HyInputMap &Input(uint32 uiIndex = 0);

private:
	void SetInputMapPtr(HyInputMap *pInputMaps);
};

#endif /* IHyApplication_h__ */
