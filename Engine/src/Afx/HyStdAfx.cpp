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
	m_uiFiltering(HYTEXFILTER_BILINEAR),
	m_uiFormat(HYTEXTURE_Uncompressed),
	m_uiFormatParam1(4),
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

	HyWindowInfo windowInfoDefault;
	windowInfoDefault.sName = "Untitled";
	windowInfoDefault.eMode = HYWINDOW_WindowedFixed;
	windowInfoDefault.vSize.x = 1280;
	windowInfoDefault.vSize.y = 756;
	windowInfoDefault.ptLocation.x = 80;
	windowInfoDefault.ptLocation.y = 80;
	windowInfoList.push_back(windowInfoDefault);

	vGravity2d = glm::vec2(0.0f, -10.0);
	fPixelsPerMeter = 50.0f;

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
	sHyProjFileName = HyIO::CleanPath(sHyProjFileName.c_str(), ".hyproj");

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
		sHyProjFileName = HyIO::CleanPath(sHyProjFileName.c_str(), ".hyproj");

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
	sProjectDir = HyIO::CleanPath(sProjectDir.c_str(), "/");
	sDataDir = HyIO::CleanPath(sDataDir.c_str(), "/");

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
	if(projDoc.HasMember("Gravity2d"))
	{
		vGravity2d.x = projDoc["Gravity2d"].GetArray()[0].GetFloat();
		vGravity2d.y = projDoc["Gravity2d"].GetArray()[1].GetFloat();
	}
	else
		vGravity2d = defaultVals.vGravity2d;
	if(projDoc.HasMember("PixelsPerMeter"))
		fPixelsPerMeter = projDoc["PixelsPerMeter"].GetFloat();
	else
		fPixelsPerMeter = defaultVals.fPixelsPerMeter;

	if(projDoc.HasMember("WindowInfo"))
	{
		HyJsonArray windowInfoArray = projDoc["WindowInfo"].GetArray();
		int uiNumWindows = windowInfoArray.Size();
		for(int32 i = 0; i < uiNumWindows; ++i)
		{
			HyJsonObj windowInfoObj = windowInfoArray[i].GetObject();

			HyWindowInfo winInfo;
			winInfo.sName = windowInfoObj["Name"].GetString();
			winInfo.eMode = static_cast<HyWindowMode>(windowInfoObj["Type"].GetInt());
			winInfo.vSize.x = windowInfoObj["ResolutionX"].GetInt();
			winInfo.vSize.y = windowInfoObj["ResolutionY"].GetInt();
			winInfo.ptLocation.x = windowInfoObj["LocationX"].GetInt();
			winInfo.ptLocation.y = windowInfoObj["LocationY"].GetInt();
			windowInfoList.push_back(winInfo);
		}
	}
	else
		windowInfoList = defaultVals.windowInfoList;

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
