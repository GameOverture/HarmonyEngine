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

HarmonyInit IHyApplication::sm_Init;

HarmonyInit::HarmonyInit()
{
	sGameName = "Untitled Game";
	sDataDir = "/data";
	eDefaultCoordinateType = HYCOORDTYPE_Camera;
	eDefaultCoordinateUnit = HYCOORDUNIT_Pixels;
	fPixelsPerMeter = 80.0f;
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
}

HarmonyInit::HarmonyInit(std::string sHyProjFilePath)
{
	std::string sProjFileContents;
	HyReadTextFile(sHyProjFilePath.c_str(), sProjFileContents);
	
	jsonxx::Object projObject;
	projObject.parse(sProjFileContents);

	sGameName				= projObject.get<jsonxx::String>("GameName");
	sDataDir				= projObject.get<jsonxx::String>("DataPath");
	eDefaultCoordinateType	= static_cast<HyCoordinateType>(static_cast<int32>(projObject.get<jsonxx::Number>("DefaultCoordinateType")));
	eDefaultCoordinateUnit	= static_cast<HyCoordinateUnit>(static_cast<int32>(projObject.get<jsonxx::Number>("DefaultCoordinateUnit")));
	fPixelsPerMeter			= static_cast<float>(projObject.get<jsonxx::Number>("PixelsPerMeter"));
	uiNumInputMappings		= static_cast<uint32>(projObject.get<jsonxx::Number>("NumInputMappings"));
	uiDebugPort				= static_cast<uint32>(projObject.get<jsonxx::Number>("DebugPort"));

	jsonxx::Array windowInfoArray = projObject.get<jsonxx::Array>("WindowInfoArray");
	uiNumWindows = static_cast<uint32>(windowInfoArray.size());
	for(uint32 i = 0; i < uiNumWindows; ++i)
	{
		jsonxx::Object windowInfoObj = windowInfoArray.get<jsonxx::Object>(i);

		windowInfo[i].sName			= windowInfoObj.get<jsonxx::String>("Name");
		windowInfo[i].eType			= static_cast<HyWindowType>(static_cast<int32>(windowInfoObj.get<jsonxx::Number>("Type")));
		windowInfo[i].vResolution.x	= static_cast<int32>(windowInfoObj.get<jsonxx::Number>("ResolutionX"));
		windowInfo[i].vResolution.y	= static_cast<int32>(windowInfoObj.get<jsonxx::Number>("ResolutionY"));
		windowInfo[i].vLocation.x	= static_cast<int32>(windowInfoObj.get<jsonxx::Number>("LocationX"));
		windowInfo[i].vLocation.y	= static_cast<int32>(windowInfoObj.get<jsonxx::Number>("LocationY"));

		windowInfo[i].uiDirtyFlags	= 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IHyApplication::IHyApplication(HarmonyInit &initStruct) : m_pInputMaps(NULL)
{
	sm_Init = initStruct;
	HyAssert(sm_Init.eDefaultCoordinateType != HYCOORDTYPE_Default, "HarmonyInit's actual 'eDefaultCoordinateType' cannot be 'HYCOORDTYPE_Default'");
	HyAssert(sm_Init.eDefaultCoordinateUnit != HYCOORDUNIT_Default, "HarmonyInit's actual 'eDefaultCoordinateUnit' cannot be 'HYCOORDUNIT_Default'");
	HyAssert(sm_Init.fPixelsPerMeter > 0.0f, "HarmonyInit's 'fPixelsPerMeter' cannot be <= 0.0f");
	
	for(uint32 i = 0; i < sm_Init.uiNumWindows; ++i)
	{
		m_WindowList.push_back(HY_NEW HyWindow());

		m_WindowList[i]->SetTitle(sm_Init.windowInfo[i].sName);
		m_WindowList[i]->SetResolution(sm_Init.windowInfo[i].vResolution);
		m_WindowList[i]->SetLocation(sm_Init.windowInfo[i].vLocation);
		m_WindowList[i]->SetType(sm_Init.windowInfo[i].eType);
	}

	//for(uint32 i = 0; i < sm_Init.uiNumInputMappings; ++i)
	//	m_InputMapList.push_back(HY_NEW HyInputMapInterop());
}

IHyApplication::~IHyApplication()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowList.size()); ++i)
		delete m_WindowList[i];
}

HyCoordinateType IHyApplication::DefaultCoordinateType()
{
	HyAssert(sm_Init.eDefaultCoordinateType != HYCOORDTYPE_Default, "HyScene::DefaultCoordinateType() invoked before engine initialized");
	return sm_Init.eDefaultCoordinateType;
}
HyCoordinateUnit IHyApplication::DefaultCoordinateUnit()
{
	HyAssert(sm_Init.eDefaultCoordinateUnit != HYCOORDUNIT_Default, "HyScene::DefaultCoordinateUnit() invoked before engine initialized");
	return sm_Init.eDefaultCoordinateUnit;
}

float IHyApplication::PixelsPerMeter()
{
	return sm_Init.fPixelsPerMeter;
}

HyWindow &IHyApplication::Window(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < sm_Init.uiNumWindows, "IApplication::Viewport() took an invalid index: " << uiIndex);
	return *m_WindowList[uiIndex];
}

uint32 IHyApplication::GetNumWindows()
{
	return sm_Init.uiNumWindows;
}

HyInputMapInterop &IHyApplication::Input(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < sm_Init.uiNumInputMappings, "IApplication::Input() took an invalid index: " << uiIndex);
	return static_cast<HyInputMapInterop &>(m_pInputMaps[uiIndex]);
}

void IHyApplication::SetInputMapPtr(HyInputMapInterop *pInputMaps)
{
	m_pInputMaps = pInputMaps;
}
