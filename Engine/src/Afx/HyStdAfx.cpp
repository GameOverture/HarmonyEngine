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

HyImageInfo::HyImageInfo(HyImageType eType, int iNumChannels, HyTextureFormat eFormat, bool bFlipVertically)
{
}

HyImageInfo::HyImageInfo(uint32 uiBucketId)
{
}

uint32 HyImageInfo::GetBucketId() const
{
}

HyImageType HyImageInfo::GetType() const
{
}

void HyImageInfo::SetType(HyImageType eType)
{
}

int HyImageInfo::GetNumChannels() const
{
}

void HyImageInfo::SetNumChannels(int iNumChannels)
{
}

HyTextureFormat HyImageInfo::GetFormat() const
{
}

void HyImageInfo::SetFormat(HyTextureFormat eFormat)
{
}

bool HyImageInfo::IsFlipVertically() const
{
}

void HyImageInfo::SetFlipVertically(bool bFlipVertically)
{
}

/*static*/ std::string HyImageInfo::GetExt(HyImageType eType)
{
	switch(eType)
	{
	case HYIMAGE_HYTX:
		return ".hytx";
	case HYIMAGE_PNG:
		return ".png";
	case HYIMAGE_DDS:
		return ".dds";
	case HYIMAGE_ASTC:
		return ".astc";
	case HYIMAGE_Unknown:
	default:
		break;
	}

	return std::string();
}

HyTextureInfo::HyTextureInfo() :
	m_uiFiltering(HYTEXFILTER_BILINEAR),
	m_uiFileType(HYTEXTUREFILE_PNG),
	m_uiFormatParam1(4),
	m_uiFormatParam2(HyTextureInfo::PackUncompressedFormatTypes(HYTEXTUREFORMAT_UINT8, HYTEXTUREFORMAT_NORM8))
{ }

HyTextureInfo::HyTextureInfo(HyTextureFiltering eFiltering, HyTextureFileType eFileType, uint8 uiFormatParam1, uint8 uiFormatParam2) :
	m_uiFiltering(eFiltering),
	m_uiFileType(eFileType),
	m_uiFormatParam1(uiFormatParam1),
	m_uiFormatParam2(uiFormatParam2)
{ }

HyTextureInfo::HyTextureInfo(uint32 uiBucketId) :
	m_uiFiltering(uiBucketId & 0xFF),
	m_uiFileType((uiBucketId & 0xFF00) >> 8),
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

HyTextureFileType HyTextureInfo::GetFileType() const
{
	return static_cast<HyTextureFileType>(m_uiFileType);
}

HyTextureFiltering HyTextureInfo::GetFiltering() const
{
	return static_cast<HyTextureFiltering>(m_uiFiltering);
}

void HyTextureInfo::GetUncompressedFormatTypes(HyTextureFormatType &eDataFormatOut, HyTextureFormatType &eInternalFormatOut) const
{
	eDataFormatOut = static_cast<HyTextureFormatType>(m_uiFormatParam2 & 0x0F);
	eInternalFormatOut = static_cast<HyTextureFormatType>((m_uiFormatParam2 & 0xF0) >> 4);
}

/*static*/ uint8 HyTextureInfo::PackUncompressedFormatTypes(HyTextureFormatType eDataFormat, HyTextureFormatType eInternalFormat)
{
	return (static_cast<uint8>(eDataFormat) | (static_cast<uint8>(eInternalFormat) << 4));
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

uint32 HyTextureInfo::GetBucketId() const
{
	return m_uiFiltering | (m_uiFileType << 8) | (m_uiFormatParam1 << 16) | (m_uiFormatParam2 << 24);
}

HyInit::HyInit()
{
	sProjectDir = ".";

	sGameName = "Untitled Game";
	sDataPath = "data";
	uiUpdatesPerSec = 0;
	uiNumInputMaps = 1;
	bShowCursor = true;

	HyWindowInfo windowInfoDefault;
	windowInfoDefault.sName = "Untitled";
	windowInfoDefault.eMode = HYWINDOW_WindowedFixed;
	windowInfoDefault.iVSync = 0;
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

HyInit::HyInit(std::string sHyProjFileName)
{
	sHyProjFileName = HyIO::CleanPath(sHyProjFileName.c_str(), ".hyproj");

	std::vector<char> sProjFileContents;
	HyIO::ReadTextFile(sHyProjFileName.c_str(), sProjFileContents);

	HyJsonDoc projDoc;
	if(projDoc.ParseInsitu(sProjFileContents.data()).HasParseError())
	{
		HyError("HyInit had JSON parsing error: " << rapidjson::GetParseErrorFunc(projDoc.GetParseError()));
		return;
	}
	HyAssert(projDoc.IsObject(), "HyInit parsed a json file that wasn't an object");

	//if(projDoc.HasMember("AdjustWorkingDirectory"))
	//{
	//	sProjectDir = projDoc["AdjustWorkingDirectory"].GetString();
	//	sHyProjFileName = sProjectDir + "/" + sHyProjFileName;
	//	sHyProjFileName = HyIO::CleanPath(sHyProjFileName.c_str(), ".hyproj");

	//	std::vector<char> trueProjFileContents;
	//	HyIO::ReadTextFile(sHyProjFileName.c_str(), trueProjFileContents);
	//	if(projDoc.ParseInsitu(trueProjFileContents.data()).HasParseError())
	//	{
	//		HyError("HyInit's AdjustWorkingDirectory had JSON parsing error: " << rapidjson::GetParseErrorFunc(projDoc.GetParseError()));
	//		return;
	//	}
	//	HyAssert(projDoc.IsObject(), "HyInit's AdjustWorkingDirectory parsed a json file that wasn't an object");

	//	sDataDir = sProjectDir + "/";
	//	sDataDir += projDoc["DataPath"].GetString();
	//}
	//else
	{
		sProjectDir = ".";
		sDataPath = projDoc["DataPath"].GetString();
	}
	sProjectDir = HyIO::CleanPath(sProjectDir.c_str(), "/");
	sDataPath = HyIO::CleanPath(sDataPath.c_str(), "/");

	HyInit defaultVals;

	if(projDoc.HasMember("Title"))
		sGameName = projDoc["Title"].GetString();
	else
		sGameName = defaultVals.sGameName;
	if(projDoc.HasMember("UpdatesPerSec"))
		uiUpdatesPerSec = projDoc["UpdatesPerSec"].GetUint();
	else
		uiUpdatesPerSec = defaultVals.uiUpdatesPerSec;
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
			if(windowInfoObj.HasMember("VSync"))
				winInfo.iVSync = windowInfoObj["VSync"].GetInt();
			else
				winInfo.iVSync = defaultVals.windowInfoList[0].iVSync;

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
