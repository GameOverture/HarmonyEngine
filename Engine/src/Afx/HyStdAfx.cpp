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

HyImageInfo::HyImageInfo() :
	m_uiWidth(0),
	m_uiHeight(0),
	m_uiType(static_cast<uint8>(HYIMAGE_Unknown)),
	m_uiChannels(static_cast<uint8>(0)),
	m_uiFormat(static_cast<uint8>(HYTEXFORMAT_Unknown)),
	m_uiVerticalFlip(false)
{ }

HyImageInfo::HyImageInfo(uint16 uiWidth, uint16 uiHeight, HyImageType eType, int iNumChannels, HyTextureFormat eFormat, bool bVerticalFlip) :
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_uiType(static_cast<uint8>(eType)),
	m_uiChannels(static_cast<uint8>(iNumChannels)),
	m_uiFormat(static_cast<uint8>(eFormat)),
	m_uiVerticalFlip(static_cast<uint8>(bVerticalFlip ? 1 : 0))
{ }

HyImageInfo::HyImageInfo(uint64 uiBucketId) :
	m_uiWidth((uiBucketId &       0xFFFF000000000000) >> 48),
	m_uiHeight((uiBucketId &      0x0000FFFF00000000) >> 32),
	m_uiType((uiBucketId &        0x00000000FF000000) >> 24),
	m_uiChannels((uiBucketId &    0x0000000000FF0000) >> 16),
	m_uiFormat((uiBucketId &      0x000000000000FF00) >> 8),
	m_uiVerticalFlip(uiBucketId & 0x00000000000000FF)
{ }

uint64 HyImageInfo::GetBucketId() const
{
	return (static_cast<uint64>(m_uiWidth) << 48) |
		   (static_cast<uint64>(m_uiHeight) << 32) |
		   (static_cast<uint64>(m_uiType) << 24) |
		   (static_cast<uint64>(m_uiChannels) << 16) |
		   (static_cast<uint64>(m_uiFormat) << 8) |
		    static_cast<uint64>(m_uiVerticalFlip);
}

uint16 HyImageInfo::GetWidth() const
{
	return m_uiWidth;
}

void HyImageInfo::SetWidth(uint16 uiWidth)
{
	 m_uiWidth = uiWidth;
}

uint16 HyImageInfo::GetHeight() const
{
	return m_uiHeight;
}

void HyImageInfo::SetHeight(uint16 uiHeight)
{
	m_uiHeight = uiHeight;
}

HyImageType HyImageInfo::GetType() const
{
	return static_cast<HyImageType>(m_uiType);
}

void HyImageInfo::SetType(HyImageType eType)
{
	m_uiType = static_cast<uint8>(eType);
}

int HyImageInfo::GetNumChannels() const
{
	return m_uiChannels;
}

void HyImageInfo::SetNumChannels(int iNumChannels)
{
	m_uiChannels = static_cast<uint8>(iNumChannels);
}

HyTextureFormat HyImageInfo::GetFormat() const
{
	return static_cast<HyTextureFormat>(m_uiFormat);
}

void HyImageInfo::SetFormat(HyTextureFormat eFormat)
{
	m_uiFormat = static_cast<uint8>(eFormat);
}

bool HyImageInfo::IsVerticalFlip() const
{
	return m_uiVerticalFlip != 0;
}

void HyImageInfo::SetVerticalFlip(bool bVerticalFlip)
{
	m_uiVerticalFlip = bVerticalFlip ? 1 : 0;
}

/*static*/ std::string HyImageInfo::GetExt(HyImageType eType)
{
	switch(eType)
	{
	case HYIMAGE_PNG:
		return ".png";
	case HYIMAGE_HYTX:
		return ".hytx";
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

HyTextureIn::HyTextureIn() :
	m_uiFilter(HYTEXFILTER_Unknown),
	m_uiWrapAndChannels(HYTEXWRAP_Unknown << 4), // NOTE: NumChannels is '0'
	m_uiFormat(HYTEXFORMAT_Unknown),
	m_uiFlags(0)
{ }

HyTextureIn::HyTextureIn(HyTextureFilter eFilter, HyTextureWrap eWrap, int iNumChannels, HyTextureFormat eFormat, uint8 uiFlags) :
	m_uiFilter(eFilter),
	m_uiWrapAndChannels((eWrap << 4) | (iNumChannels & 0x0F)),
	m_uiFormat(eFormat),
	m_uiFlags(uiFlags)
{ }

HyTextureIn::HyTextureIn(uint32 uiBucketId) :
	m_uiFilter((uiBucketId &          0xFF000000) >> 24),
	m_uiWrapAndChannels((uiBucketId & 0x00FF0000) >> 16),
	m_uiFormat((uiBucketId &          0x0000FF00) >> 8),
	m_uiFlags(uiBucketId &            0x000000FF)
{ }

uint32 HyTextureIn::GetBucketId() const
{
	return (static_cast<uint32>(m_uiFilter) << 24) |
		   (static_cast<uint32>(m_uiWrapAndChannels) << 16) |
		   (static_cast<uint32>(m_uiFormat) << 8) |
		    static_cast<uint32>(m_uiFlags);
}

HyTextureFilter HyTextureIn::GetFilter() const
{
	return static_cast<HyTextureFilter>(m_uiFilter);
}

void HyTextureIn::SetFilter(HyTextureFilter eFilter)
{
	m_uiFilter = static_cast<uint8>(eFilter);
}

HyTextureWrap HyTextureIn::GetWrap() const
{
	return static_cast<HyTextureWrap>((m_uiWrapAndChannels & 0xF0) >> 4);
}

void HyTextureIn::SetWrap(HyTextureWrap eWrap)
{
	m_uiWrapAndChannels &= ~0xF0;
	m_uiWrapAndChannels |= static_cast<uint8>(eWrap << 4);
}

int HyTextureIn::GetNumChannels() const
{
	return (m_uiWrapAndChannels & 0x0F);
}

void HyTextureIn::SetNumChannels(int iNumChannels)
{
	m_uiWrapAndChannels &= ~0x0F;
	m_uiWrapAndChannels |= static_cast<uint8>(iNumChannels & 0x0000000F);
}

HyTextureFormat HyTextureIn::GetFormat() const
{
	return static_cast<HyTextureFormat>(m_uiFormat);
}

void HyTextureIn::SetFormat(HyTextureFormat eFormat)
{
	m_uiFormat = static_cast<uint8>(eFormat);
}

uint8 HyTextureIn::GetFlags() const
{
	return m_uiFlags;
}

void HyTextureIn::SetFlags(uint8 uiFlags)
{
	m_uiFlags = uiFlags;
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
