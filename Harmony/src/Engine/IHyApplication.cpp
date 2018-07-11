/**************************************************************************
 *	IHyApplication.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IHyApplication.h"

#include "Renderer/Components/HyWindow.h"
#include "Utilities/HyStrManip.h"

#ifdef HY_PLATFORM_DESKTOP
void glfw_ErrorCallback(int iError, const char *szDescription)
{
	HyLogError("GLFW Error: " << iError << "\n" << szDescription);
}
#endif

HarmonyInit::HarmonyInit()
{
	sProjectDir = ".";

	sGameName = "Untitled Game";
	sDataDir = "/data";
	uiUpdateTickMs = 0;
	fPixelsPerMeter = 80.0f;
	bShowCursor = true;
	uiNumInputMappings = 1;
	uiDebugPort = 1313;

	uiNumWindows = 1;
	for(uint32 i = 0; i < HY_MAXWINDOWS; ++i)
	{
		windowInfo[i].sName = "Window: " + std::to_string(i);
		windowInfo[i].eType = HYWINDOW_WindowedFixed;
		windowInfo[i].vSize.x = 512;
		windowInfo[i].vSize.y = 256;
		windowInfo[i].ptLocation.x = i * windowInfo[i].vSize.x;
		windowInfo[i].ptLocation.y = 0;
	}

	bUseConsole = false;
	consoleInfo.sName = "Harmony Log Console";
	consoleInfo.eType = HYWINDOW_WindowedSizeable;
	consoleInfo.vSize.x = 64;
	consoleInfo.vSize.y = 80;
	consoleInfo.ptLocation.x = 512;
	consoleInfo.ptLocation.y = 256;
}

HarmonyInit::HarmonyInit(std::string sHyProjFileName)
{
	sHyProjFileName = HyStr::MakeStringProperPath(sHyProjFileName.c_str(), ".hyproj", false);

	std::string sProjFileContents;
	HyReadTextFile(sHyProjFileName.c_str(), sProjFileContents);
	
	jsonxx::Object projObject;
	projObject.parse(sProjFileContents);

	if(projObject.has<jsonxx::String>("AdjustWorkingDirectory"))
	{
		sProjectDir = projObject.get<jsonxx::String>("AdjustWorkingDirectory");
		sHyProjFileName = sProjectDir + "/" + sHyProjFileName;
		sHyProjFileName = HyStr::MakeStringProperPath(sHyProjFileName.c_str(), ".hyproj", false);

		HyReadTextFile(sHyProjFileName.c_str(), sProjFileContents);
		projObject.parse(sProjFileContents);

		sDataDir = sProjectDir + "/" + projObject.get<jsonxx::String>("DataPath");
	}
	else
	{
		sProjectDir = ".";
		sDataDir = projObject.get<jsonxx::String>("DataPath");
	}

	sProjectDir = HyStr::MakeStringProperPath(sProjectDir.c_str(), "", true);
	sDataDir = HyStr::MakeStringProperPath(sDataDir.c_str(), "", true);
	
	sGameName				= projObject.get<jsonxx::String>("GameName");
	uiUpdateTickMs			= static_cast<uint32>(projObject.get<jsonxx::Number>("UpdateFpsCap")); // TODO: Change name
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
			windowInfo[i].vSize.x = static_cast<int32>(windowInfoObj.get<jsonxx::Number>("ResolutionX"));
			windowInfo[i].vSize.y = static_cast<int32>(windowInfoObj.get<jsonxx::Number>("ResolutionY"));
			windowInfo[i].ptLocation.x = static_cast<int32>(windowInfoObj.get<jsonxx::Number>("LocationX"));
			windowInfo[i].ptLocation.y = static_cast<int32>(windowInfoObj.get<jsonxx::Number>("LocationY"));
		}
	}
	else
	{
		uiNumWindows = 1;
		for(uint32 i = 0; i < HY_MAXWINDOWS; ++i)
		{
			windowInfo[i].sName = "Window: " + std::to_string(i);
			windowInfo[i].eType = HYWINDOW_WindowedFixed;
			windowInfo[i].vSize.x = 512;
			windowInfo[i].vSize.y = 256;
			windowInfo[i].ptLocation.x = i * windowInfo[i].vSize.x;
			windowInfo[i].ptLocation.y = 0;
		}
	}

	if(projObject.has<jsonxx::Boolean>("UseConsole") == true)
	{
		// Log Console
		bUseConsole = projObject.get<jsonxx::Boolean>("UseConsole");
		jsonxx::Object consoleInfoObj = projObject.get<jsonxx::Object>("ConsoleInfo");
		consoleInfo.sName = consoleInfoObj.get<jsonxx::String>("Name");
		consoleInfo.eType = static_cast<HyWindowType>(static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("Type")));
		consoleInfo.vSize.x = static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("ResolutionX"));
		consoleInfo.vSize.y = static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("ResolutionY"));
		consoleInfo.ptLocation.x = static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("LocationX"));
		consoleInfo.ptLocation.y = static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("LocationY"));
	}
	else
	{
		bUseConsole = false;
		consoleInfo.sName = "Harmony Log Console";
		consoleInfo.eType = HYWINDOW_WindowedSizeable;
		consoleInfo.vSize.x = 64;
		consoleInfo.vSize.y = 80;
		consoleInfo.ptLocation.x = 512;
		consoleInfo.ptLocation.y = 256;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IHyApplication::IHyApplication(HarmonyInit &initStruct) :	m_pInputMaps(nullptr),
															m_Console(initStruct.bUseConsole, initStruct.consoleInfo)
{
	HyAssert(m_Init.fPixelsPerMeter > 0.0f, "HarmonyInit's 'fPixelsPerMeter' cannot be <= 0.0f");

#ifdef HY_PLATFORM_DESKTOP
	// Setup error callback before glfwInit to catch anything that might go wrong with glfwInit
	glfwSetErrorCallback(glfw_ErrorCallback);

	if(glfwInit() == GLFW_FALSE)
		HyLogError("glfwInit failed");
#endif
	
	m_Init = initStruct;
	
	for(uint32 i = 0; i < m_Init.uiNumWindows; ++i)
		m_WindowList.push_back(HY_NEW HyWindow(i, m_Init.windowInfo[i], m_Init.bShowCursor, i != 0 ? m_WindowList[0]->GetHandle() : nullptr));
}

IHyApplication::~IHyApplication()
{
#ifdef HY_PLATFORM_DESKTOP
	glfwTerminate();
#endif

	for(uint32 i = 0; i < static_cast<uint32>(m_WindowList.size()); ++i)
		delete m_WindowList[i];
}

std::string IHyApplication::GameName() const
{
	return m_Init.sGameName;
}

std::string IHyApplication::DataDir() const
{
	return m_Init.sDataDir;
}

bool IHyApplication::IsShowCursor() const
{
	return m_Init.bShowCursor;
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

HyInputMap &IHyApplication::Input(uint32 uiIndex /*= 0*/)
{
	HyAssert(m_pInputMaps && uiIndex < m_Init.uiNumInputMappings, "IApplication::Input() took an invalid index: " << uiIndex);
	return static_cast<HyInputMap &>(m_pInputMaps[uiIndex]);
}

void IHyApplication::SetInputMapPtr(HyInputMap *pInputMaps)
{
	m_pInputMaps = pInputMaps;
}
