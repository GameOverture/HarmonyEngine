/**************************************************************************
 *	HyFileIO.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyFileIO_h__
#define __HyFileIO_h__

#include "Afx/HyStdAfx.h"
#include "FileIO/jsonxx.h"

class HyFileIO
{
	static std::string		sm_sDataDir;

	static std::string		sm_sLogStr;

public:
	static void SetDataDir(const char *szPath);
	static const std::string &GetDataDir()			{ return sm_sDataDir; }

	// Do not include the file extension in 'szName'
	static std::string GetFilePath(HyInstanceType eDataDirType, const char *szPrefix, const char *szName);

	static bool FileExists(const std::string &sFilePath);

	static char *ReadTextFile(const char *szFilePath, int *iLength);
	static std::string ReadTextFile(const char *szFilePath);

	static jsonxx::Object ReadJsonFile(HyInstanceType eDataDir, const char *szPrefix, const char *szName);
};

#endif /* __HyFileIO_h__ */
