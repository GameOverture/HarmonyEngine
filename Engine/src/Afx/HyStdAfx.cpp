/**************************************************************************
 *	HyStdAfx.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/

// This cpp module includes HyStdAfx.h to create the precompiled header
#include "Afx/HyStdAfx.h"
#include "Utilities/HyIO.h"

HarmonyInit::HarmonyInit()
{
	sProjectDir = ".";

	sGameName = "Untitled Game";
	sDataDir = "data";
	uiUpdateTickMs = 0;
	bShowCursor = true;
	uiNumInputMappings = 1;
	uiDebugPort = 1313;

	uiNumWindows = 1;
	for(uint32 i = 0; i < HY_MAXWINDOWS; ++i)
	{
		windowInfo[i].sName = "Window: " + std::to_string(i);
		windowInfo[i].eType = HYWINDOW_WindowedFixed;
		windowInfo[i].vSize.x = 1280;
		windowInfo[i].vSize.y = 756;
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
	sHyProjFileName = HyIO::CleanPath(sHyProjFileName.c_str(), ".hyproj", false);

	std::vector<char> sProjFileContents;
	HyIO::ReadTextFile(sHyProjFileName.c_str(), sProjFileContents);

	HyJsonDoc projDoc;
	if(projDoc.ParseInsitu(sProjFileContents.data()).HasParseError())
	{
		HyError("HarmonyInit had JSON parsing error: " << rapidjson::GetParseErrorFunc(projDoc.GetParseError()));
		return;
	}
	HyAssert(projDoc.IsObject(), "HarmonyInit parsed a json file that wasn't an object");

	if(projDoc.HasMember("AdjustWorkingDirectory"))
	{
		sProjectDir = projDoc["AdjustWorkingDirectory"].GetString();
		sHyProjFileName = sProjectDir + "/" + sHyProjFileName;
		sHyProjFileName = HyIO::CleanPath(sHyProjFileName.c_str(), ".hyproj", false);

		std::vector<char> trueProjFileContents;
		HyIO::ReadTextFile(sHyProjFileName.c_str(), trueProjFileContents);
		if(projDoc.ParseInsitu(trueProjFileContents.data()).HasParseError())
		{
			HyError("HarmonyInit's AdjustWorkingDirectory had JSON parsing error: " << rapidjson::GetParseErrorFunc(projDoc.GetParseError()));
			return;
		}
		HyAssert(projDoc.IsObject(), "HarmonyInit's AdjustWorkingDirectory parsed a json file that wasn't an object");

		sDataDir = sProjectDir + "/";
		sDataDir += projDoc["DataPath"].GetString();
	}
	else
	{
		sProjectDir = ".";
		sDataDir = projDoc["DataPath"].GetString();
	}

	sProjectDir = HyIO::CleanPath(sProjectDir.c_str(), "", true);
	sDataDir = HyIO::CleanPath(sDataDir.c_str(), "", true);
	
	sGameName				= projDoc["GameName"].GetString();
	uiUpdateTickMs			= projDoc["UpdateFpsCap"].GetUint();
	bShowCursor				= projDoc["ShowCursor"].GetBool();
	uiNumInputMappings		= projDoc["NumInputMappings"].GetUint();
	uiDebugPort				= projDoc["DebugPort"].GetUint();

	if(projDoc.HasMember("WindowInfoArray"))
	{
		HyJsonArray windowInfoArray = projDoc["WindowInfoArray"].GetArray();
		uiNumWindows = windowInfoArray.Size();
		for(uint32 i = 0; i < uiNumWindows; ++i)
		{
			HyJsonObj windowInfoObj = windowInfoArray[i].GetObjectA();

			windowInfo[i].sName = windowInfoObj["Name"].GetString();
			windowInfo[i].eType = static_cast<HyWindowType>(windowInfoObj["Type"].GetInt());
			windowInfo[i].vSize.x = windowInfoObj["ResolutionX"].GetInt();
			windowInfo[i].vSize.y = windowInfoObj["ResolutionY"].GetInt();
			windowInfo[i].ptLocation.x = windowInfoObj["LocationX"].GetInt();
			windowInfo[i].ptLocation.y = windowInfoObj["LocationY"].GetInt();
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

	if(projDoc.HasMember("UseConsole"))// projObject.has<jsonxx::Boolean>("UseConsole") == true)
	{
		// Log Console
		bUseConsole = projDoc["UseConsole"].GetBool();// projObject.get<jsonxx::Boolean>("UseConsole");
		
		//HyJsonObj consoleInfoObj = projObject.get<HyJsonObj>("ConsoleInfo");
		HyJsonObj consoleInfoObj = projDoc["ConsoleInfo"].GetObjectA();
		consoleInfo.sName = consoleInfoObj["Name"].GetString();// consoleInfoObj.get<jsonxx::String>("Name");
		consoleInfo.eType = static_cast<HyWindowType>(consoleInfoObj["Type"].GetInt());// static_cast<HyWindowType>(static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("Type")));
		consoleInfo.vSize.x = consoleInfoObj["ResolutionX"].GetInt();// static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("ResolutionX"));
		consoleInfo.vSize.y = consoleInfoObj["ResolutionY"].GetInt();//static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("ResolutionY"));
		consoleInfo.ptLocation.x = consoleInfoObj["LocationX"].GetInt();//static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("LocationX"));
		consoleInfo.ptLocation.y = consoleInfoObj["LocationY"].GetInt();//static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("LocationY"));
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
