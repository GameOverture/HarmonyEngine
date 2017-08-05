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

#include "Renderer/Components/HyWindow.h"
#include "Utilities/HyStrManip.h"

HyCoordinateUnit IHyApplication::sm_eDefaultCoordinateUnit = HYCOORDUNIT_Default;
float IHyApplication::sm_fPixelsPerMeter = 0.0f;

HarmonyInit::HarmonyInit()
{
	sGameName = "Untitled Game";
	sDataDir = "/data";
	eDefaultCoordinateUnit = HYCOORDUNIT_Pixels;
	uiUpdateFpsCap = 0;
	fPixelsPerMeter = 80.0f;
	bShowCursor = true;
	uiNumInputMappings = 1;
	uiDebugPort = 1313;

	uiNumWindows = 1;
	for(uint32 i = 0; i < HY_MAXWINDOWS; ++i)
	{
		windowInfo[i].sName = "Window: " + std::to_string(i);
		windowInfo[i].eType = HYWINDOW_WindowedFixed;
		windowInfo[i].vResolution.x = 512;
		windowInfo[i].vResolution.y = 256;
		windowInfo[i].vLocation.x = i * windowInfo[i].vResolution.x;
		windowInfo[i].vLocation.y = 0;

		windowInfo[i].uiDirtyFlags = 0;
	}

	bUseConsole = false;
	consoleInfo.sName = "Harmony Log Console";
	consoleInfo.eType = HYWINDOW_WindowedSizeable;
	consoleInfo.vResolution.x = 64;
	consoleInfo.vResolution.y = 80;
	consoleInfo.vLocation.x = 512;
	consoleInfo.vLocation.y = 256;
	consoleInfo.uiDirtyFlags = 0;
}

HarmonyInit::HarmonyInit(std::string sHyProjFileName)
{
	sHyProjFileName = MakeStringProperPath(sHyProjFileName.c_str(), ".hyproj", false);

	std::string sProjFileContents;
	HyReadTextFile(sHyProjFileName.c_str(), sProjFileContents);
	
	jsonxx::Object projObject;
	projObject.parse(sProjFileContents);

	if(projObject.has<jsonxx::String>("AdjustWorkingDirectory"))
	{
		std::string sWorkingDir = projObject.get<jsonxx::String>("AdjustWorkingDirectory");
		HyError("HarmonyInit::HarmonyInit - AdjustWorkingDirectory not implemented yet.");
	}

	sGameName				= projObject.get<jsonxx::String>("GameName");
	sDataDir				= projObject.get<jsonxx::String>("DataPath");
	eDefaultCoordinateUnit	= static_cast<HyCoordinateUnit>(static_cast<int32>(projObject.get<jsonxx::Number>("DefaultCoordinateUnit")));
	uiUpdateFpsCap			= static_cast<uint32>(projObject.get<jsonxx::Number>("UpdateFpsCap"));
	fPixelsPerMeter			= static_cast<float>(projObject.get<jsonxx::Number>("PixelsPerMeter"));
	bShowCursor				= projObject.get<jsonxx::Boolean>("ShowCursor");
	uiNumInputMappings		= static_cast<uint32>(projObject.get<jsonxx::Number>("NumInputMappings"));
	uiDebugPort				= static_cast<uint32>(projObject.get<jsonxx::Number>("DebugPort"));

	if(projObject.has<jsonxx::Array>("WindowInfoArray") == true)
	{
		jsonxx::Array windowInfoArray = projObject.get<jsonxx::Array>("WindowInfoArray");
		uiNumWindows = static_cast<uint32>(windowInfoArray.size());
		for(uint32 i = 0; i < uiNumWindows; ++i)
		{
			jsonxx::Object windowInfoObj = windowInfoArray.get<jsonxx::Object>(i);

			windowInfo[i].sName = windowInfoObj.get<jsonxx::String>("Name");
			windowInfo[i].eType = static_cast<HyWindowType>(static_cast<int32>(windowInfoObj.get<jsonxx::Number>("Type")));
			windowInfo[i].vResolution.x = static_cast<int32>(windowInfoObj.get<jsonxx::Number>("ResolutionX"));
			windowInfo[i].vResolution.y = static_cast<int32>(windowInfoObj.get<jsonxx::Number>("ResolutionY"));
			windowInfo[i].vLocation.x = static_cast<int32>(windowInfoObj.get<jsonxx::Number>("LocationX"));
			windowInfo[i].vLocation.y = static_cast<int32>(windowInfoObj.get<jsonxx::Number>("LocationY"));

			windowInfo[i].uiDirtyFlags = 0;
		}
	}
	else
	{
		uiNumWindows = 1;
		for(uint32 i = 0; i < HY_MAXWINDOWS; ++i)
		{
			windowInfo[i].sName = "Window: " + std::to_string(i);
			windowInfo[i].eType = HYWINDOW_WindowedFixed;
			windowInfo[i].vResolution.x = 512;
			windowInfo[i].vResolution.y = 256;
			windowInfo[i].vLocation.x = i * windowInfo[i].vResolution.x;
			windowInfo[i].vLocation.y = 0;

			windowInfo[i].uiDirtyFlags = 0;
		}
	}

	if(projObject.has<jsonxx::Boolean>("UseConsole") == true)
	{
		// Log Console
		bUseConsole = projObject.get<jsonxx::Boolean>("UseConsole");
		jsonxx::Object consoleInfoObj = projObject.get<jsonxx::Object>("ConsoleInfo");
		consoleInfo.sName = consoleInfoObj.get<jsonxx::String>("Name");
		consoleInfo.eType = static_cast<HyWindowType>(static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("Type")));
		consoleInfo.vResolution.x = static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("ResolutionX"));
		consoleInfo.vResolution.y = static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("ResolutionY"));
		consoleInfo.vLocation.x = static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("LocationX"));
		consoleInfo.vLocation.y = static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("LocationY"));
		consoleInfo.uiDirtyFlags = 0;
	}
	else
	{
		bUseConsole = false;
		consoleInfo.sName = "Harmony Log Console";
		consoleInfo.eType = HYWINDOW_WindowedSizeable;
		consoleInfo.vResolution.x = 64;
		consoleInfo.vResolution.y = 80;
		consoleInfo.vLocation.x = 512;
		consoleInfo.vLocation.y = 256;
		consoleInfo.uiDirtyFlags = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IHyApplication::IHyApplication(HarmonyInit &initStruct) :	m_pInputMaps(NULL),
															m_Console(initStruct.bUseConsole, initStruct.consoleInfo)
{
	HyAssert(m_Init.eDefaultCoordinateUnit != HYCOORDUNIT_Default, "HarmonyInit's actual 'eDefaultCoordinateUnit' cannot be 'HYCOORDUNIT_Default'");
	HyAssert(m_Init.fPixelsPerMeter > 0.0f, "HarmonyInit's 'fPixelsPerMeter' cannot be <= 0.0f");
	
	m_Init = initStruct;
	sm_eDefaultCoordinateUnit = m_Init.eDefaultCoordinateUnit;
	sm_fPixelsPerMeter = m_Init.fPixelsPerMeter;
	
	for(uint32 i = 0; i < m_Init.uiNumWindows; ++i)
	{
		m_WindowList.push_back(HY_NEW HyWindow());

		m_WindowList[i]->SetTitle(m_Init.windowInfo[i].sName);
		m_WindowList[i]->SetResolution(m_Init.windowInfo[i].vResolution);
		m_WindowList[i]->SetLocation(m_Init.windowInfo[i].vLocation);
		m_WindowList[i]->SetType(m_Init.windowInfo[i].eType);
	}

	//for(uint32 i = 0; i < sm_Init.uiNumInputMappings; ++i)
	//	m_InputMapList.push_back(HY_NEW HyInputMapInterop());
}

IHyApplication::~IHyApplication()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowList.size()); ++i)
		delete m_WindowList[i];
}

HyCoordinateUnit IHyApplication::DefaultCoordinateUnit()
{
	HyAssert(sm_eDefaultCoordinateUnit != HYCOORDUNIT_Default, "HyScene::DefaultCoordinateUnit() invoked before IHyApplication initialized");
	return sm_eDefaultCoordinateUnit;
}

float IHyApplication::PixelsPerMeter()
{
	HyAssert(sm_fPixelsPerMeter > 0.0f, "HyScene::PixelsPerMeter() invoked before IHyApplication initialized");
	return sm_fPixelsPerMeter;
}

std::string IHyApplication::GameName() const
{
	return m_Init.sGameName;
}

HyWindow &IHyApplication::Window(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < m_Init.uiNumWindows, "IApplication::Viewport() took an invalid index: " << uiIndex);
	return *m_WindowList[uiIndex];
}

uint32 IHyApplication::GetNumWindows()
{
	return m_Init.uiNumWindows;
}

HyInputMapInterop &IHyApplication::Input(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < m_Init.uiNumInputMappings, "IApplication::Input() took an invalid index: " << uiIndex);
	return static_cast<HyInputMapInterop &>(m_pInputMaps[uiIndex]);
}

void IHyApplication::SetInputMapPtr(HyInputMapInterop *pInputMaps)
{
	m_pInputMaps = pInputMaps;
}
