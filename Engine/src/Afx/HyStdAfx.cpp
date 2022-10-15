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
#include "Utilities/HyJson.h"

HyTextureInfo::HyTextureInfo() :
	m_uiFiltering(HYTEXFILTER_Unknown),
	m_uiFormat(HYTEXFILTER_Unknown),
	m_uiFormatParam1(0),
	m_uiFormatParam2(0)
{ }

HyTextureInfo::HyTextureInfo(HyTextureFiltering eFiltering, HyTextureFormat eFormat, uint8 uiFormatParam1, uint8 uiFormatParam2) :
	m_uiFiltering(eFiltering),
	m_uiFormat(eFormat),
	m_uiFormatParam1(uiFormatParam1),
	m_uiFormatParam2(uiFormatParam2)
{ }

HyTextureInfo::HyTextureInfo(uint32 uiBucketId) :
	m_uiFiltering(uiBucketId & 0xFF),
	m_uiFormat((uiBucketId & 0xFF00) >> 8),
	m_uiFormatParam1((uiBucketId & 0xFF0000) >> 16),
	m_uiFormatParam2((uiBucketId & 0xFF000000) >> 24)
{ }

bool HyTextureInfo::operator==(const HyTextureInfo &rhs) const
{
	return GetBucketId() == rhs.GetBucketId();
}

bool HyTextureInfo::operator!=(const HyTextureInfo &rhs) const
{
	return GetBucketId() != rhs.GetBucketId();
}

HyTextureFormat HyTextureInfo::GetFormat() const
{
	return static_cast<HyTextureFormat>(m_uiFormat);
}

HyTextureFiltering HyTextureInfo::GetFiltering() const
{
	return static_cast<HyTextureFiltering>(m_uiFiltering);
}

bool HyTextureInfo::IsMipMaps() const
{
	switch(static_cast<HyTextureFiltering>(m_uiFiltering))
	{
	case HYTEXFILTER_NEAREST_MIPMAP:
	case HYTEXFILTER_LINEAR_MIPMAP:
	case HYTEXFILTER_BILINEAR_MIPMAP:
	case HYTEXFILTER_TRILINEAR:
		return true;

	default:
		return false;
	}
}

std::string HyTextureInfo::GetFileExt() const
{
	switch(m_uiFormat)
	{
	case HYTEXTURE_Uncompressed:
		switch(m_uiFormatParam2)
		{
		case UNCOMPRESSEDFILE_PNG:
			return ".png";
		default:
			return ".xxx";
		}
	case HYTEXTURE_DXT:
		return ".dds";
	case HYTEXTURE_ASTC:
		return ".astc";

	case HYTEXTURE_Unknown:
	default:
		return ".xxx";
	}
}

uint32 HyTextureInfo::GetBucketId() const
{
	return m_uiFiltering | (m_uiFormat << 8) | (m_uiFormatParam1 << 16) | (m_uiFormatParam2 << 24);
}

HarmonyInit::HarmonyInit()
{
	sProjectDir = ".";

	sGameName = "Untitled Game";
	sDataDir = "data";
	uiUpdatesPerSec = 0;
	iVSync = 1;
	uiNumInputMaps = 1;
	bShowCursor = true;

	uiNumWindows = 1;
	for(uint32 i = 0; i < HY_MAXWINDOWS; ++i)
	{
		windowInfo[i].sName = "Window: " + std::to_string(i);
		windowInfo[i].eMode = HYWINDOW_WindowedFixed;
		windowInfo[i].vSize.x = 1280;
		windowInfo[i].vSize.y = 756;
		windowInfo[i].ptLocation.x = i * windowInfo[i].vSize.x;
		windowInfo[i].ptLocation.y = 80;
	}

	bUseConsole = false;
	consoleInfo.sName = "Harmony Log Console";
	consoleInfo.eMode = HYWINDOW_WindowedSizeable;
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

	HarmonyInit defaultVals;
	
	if(projDoc.HasMember("Title"))
		sGameName = projDoc["Title"].GetString();
	else
		sGameName = defaultVals.sGameName;
	if(projDoc.HasMember("UpdatesPerSec"))
		uiUpdatesPerSec = projDoc["UpdatesPerSec"].GetUint();
	else
		uiUpdatesPerSec = defaultVals.uiUpdatesPerSec;
	if(projDoc.HasMember("VSync"))
		iVSync = projDoc["VSync"].GetInt();
	else
		iVSync = defaultVals.iVSync;
	if(projDoc.HasMember("NumInputMaps"))
		uiNumInputMaps = projDoc["NumInputMaps"].GetUint();
	else
		uiNumInputMaps = defaultVals.uiNumInputMaps;
	if(projDoc.HasMember("ShowCursor"))
		bShowCursor = projDoc["ShowCursor"].GetBool();
	else
		bShowCursor = defaultVals.bShowCursor;

	uiNumWindows = defaultVals.uiNumWindows;
	for(uint32 i = 0; i < HY_MAXWINDOWS; ++i)
		windowInfo[i] = defaultVals.windowInfo[i];

	if(projDoc.HasMember("WindowInfo"))
	{
		HyJsonArray windowInfoArray = projDoc["WindowInfo"].GetArray();
		uiNumWindows = windowInfoArray.Size();
		for(uint32 i = 0; i < uiNumWindows; ++i)
		{
			HyJsonObj windowInfoObj = windowInfoArray[i].GetObject();

			windowInfo[i].sName = windowInfoObj["Name"].GetString();
			windowInfo[i].eMode = static_cast<HyWindowMode>(windowInfoObj["Type"].GetInt());
			windowInfo[i].vSize.x = windowInfoObj["ResolutionX"].GetInt();
			windowInfo[i].vSize.y = windowInfoObj["ResolutionY"].GetInt();
			windowInfo[i].ptLocation.x = windowInfoObj["LocationX"].GetInt();
			windowInfo[i].ptLocation.y = windowInfoObj["LocationY"].GetInt();
		}
	}

	bUseConsole = defaultVals.bUseConsole;
	consoleInfo = defaultVals.consoleInfo;
	if(projDoc.HasMember("UseConsole"))// projObject.has<jsonxx::Boolean>("UseConsole") == true)
	{
		// Log Console
		bUseConsole = projDoc["UseConsole"].GetBool();// projObject.get<jsonxx::Boolean>("UseConsole");
		
		//HyJsonObj consoleInfoObj = projObject.get<HyJsonObj>("ConsoleInfo");
		HyJsonObj consoleInfoObj = projDoc["ConsoleInfo"].GetObject();
		consoleInfo.sName = consoleInfoObj["Name"].GetString();// consoleInfoObj.get<jsonxx::String>("Name");
		consoleInfo.eMode = static_cast<HyWindowMode>(consoleInfoObj["Type"].GetInt());// static_cast<HyWindowType>(static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("Type")));
		consoleInfo.vSize.x = consoleInfoObj["ResolutionX"].GetInt();// static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("ResolutionX"));
		consoleInfo.vSize.y = consoleInfoObj["ResolutionY"].GetInt();//static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("ResolutionY"));
		consoleInfo.ptLocation.x = consoleInfoObj["LocationX"].GetInt();//static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("LocationX"));
		consoleInfo.ptLocation.y = consoleInfoObj["LocationY"].GetInt();//static_cast<int32>(consoleInfoObj.get<jsonxx::Number>("LocationY"));
	}
}
