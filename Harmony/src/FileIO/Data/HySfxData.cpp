/**************************************************************************
 *	HySfxData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FileIO/Data/HySfxData.h"


HySfxData::HySfxData(const std::string &sPath) : IHyData(HYINST_Sound2d, sPath)
{
}

HySfxData::~HySfxData(void)
{
}

/*virtual*/ void HySfxData::DoFileLoad()
{
	std::string sFilePath;
	//HyFileIO::GetDataDir(DIR_Audio, sFilePath);
	//sFilePath += GetName();
	//sFilePath += ".wav";

	//if(!m_SoundBuffer.loadFromFile(sFilePath))
	//	HyError("HySfxData::DoFileLoad() Cannot load from: " << sFilePath.c_str());
}
