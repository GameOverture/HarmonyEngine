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

	std::string sProjFileContents;
	HyIO::ReadTextFile(sHyProjFileName.c_str(), sProjFileContents);

	HyJsonDoc projDoc;
	if(projDoc.Parse(sProjFileContents.c_str()).HasParseError())
	{
		HyError("HarmonyInit had JSON parsing error: " << rapidjson::GetParseErrorFunc(projDoc.GetParseError()));
		return;
	}
	HyAssert(projDoc.IsObject(), "HarmonyInit parsed a json file that wasn't an object");
	
	//jsonxx::Object projObject;
	//projObject.parse(sProjFileContents);

	if(projDoc.HasMember("AdjustWorkingDirectory")) //projObject.has<jsonxx::String>("AdjustWorkingDirectory"))
	{
		sProjectDir = projDoc["AdjustWorkingDirectory"].GetString();
		//sProjectDir = projObject.get<jsonxx::String>("AdjustWorkingDirectory");
		sHyProjFileName = sProjectDir + "/" + sHyProjFileName;
		sHyProjFileName = HyIO::CleanPath(sHyProjFileName.c_str(), ".hyproj", false);

		HyIO::ReadTextFile(sHyProjFileName.c_str(), sProjFileContents);
		if(projDoc.Parse(sProjFileContents.c_str()).HasParseError())
		{
			HyError("HarmonyInit's AdjustWorkingDirectory had JSON parsing error: " << rapidjson::GetParseErrorFunc(projDoc.GetParseError()));
			return;
		}
		HyAssert(projDoc.IsObject(), "HarmonyInit's AdjustWorkingDirectory parsed a json file that wasn't an object");

		//projObject.parse(sProjFileContents);

		sDataDir = sProjectDir + "/";
		sDataDir += projDoc["DataPath"].GetString();//projObject.get<jsonxx::String>("DataPath");
	}
	else
	{
		sProjectDir = ".";
		sDataDir = projDoc["DataPath"].GetString();//projObject.get<jsonxx::String>("DataPath");
	}

	sProjectDir = HyIO::CleanPath(sProjectDir.c_str(), "", true);
	sDataDir = HyIO::CleanPath(sDataDir.c_str(), "", true);
	
	sGameName				= projDoc["GameName"].GetString();// projObject.get<jsonxx::String>("GameName");
	uiUpdateTickMs			= projDoc["UpdateFpsCap"].GetUint();// static_cast<uint32>(projObject.get<jsonxx::Number>("UpdateFpsCap")); // TODO: Change name
	bShowCursor				= projDoc["ShowCursor"].GetBool();// projObject.get<jsonxx::Boolean>("ShowCursor");
	uiNumInputMappings		= projDoc["NumInputMappings"].GetUint();// static_cast<uint32>(projObject.get<jsonxx::Number>("NumInputMappings"));
	uiDebugPort				= projDoc["DebugPort"].GetUint();// static_cast<uint32>(projObject.get<jsonxx::Number>("DebugPort"));

	if(projDoc.HasMember("WindowInfoArray"))// projObject.has<jsonxx::Array>("WindowInfoArray") == true)
	{
		//jsonxx::Array windowInfoArray = projObject.get<jsonxx::Array>("WindowInfoArray");
		HyJsonArray windowInfoArray = projDoc["WindowInfoArray"].GetArray();
		uiNumWindows = windowInfoArray.Size();// static_cast<uint32>(windowInfoArray.size());
		for(uint32 i = 0; i < uiNumWindows; ++i)
		{
			//jsonxx::Object windowInfoObj = windowInfoArray.get<jsonxx::Object>(i);
			HyJsonObj windowInfoObj = windowInfoArray[i].GetObjectA();

			windowInfo[i].sName = windowInfoObj["Name"].GetString();// windowInfoObj.get<jsonxx::String>("Name");
			windowInfo[i].eType = static_cast<HyWindowType>(windowInfoObj["Type"].GetInt());// static_cast<HyWindowType>(static_cast<int32>(windowInfoObj.get<jsonxx::Number>("Type")));
			windowInfo[i].vSize.x = windowInfoObj["ResolutionX"].GetInt();// static_cast<int32>(windowInfoObj.get<jsonxx::Number>("ResolutionX"));
			windowInfo[i].vSize.y = windowInfoObj["ResolutionY"].GetInt();// static_cast<int32>(windowInfoObj.get<jsonxx::Number>("ResolutionY"));
			windowInfo[i].ptLocation.x = windowInfoObj["LocationX"].GetInt();//static_cast<int32>(windowInfoObj.get<jsonxx::Number>("LocationX"));
			windowInfo[i].ptLocation.y = windowInfoObj["LocationY"].GetInt();//static_cast<int32>(windowInfoObj.get<jsonxx::Number>("LocationY"));
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
		
		//jsonxx::Object consoleInfoObj = projObject.get<jsonxx::Object>("ConsoleInfo");
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
