/**************************************************************************
*	HyFileIO.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyFileIO_h__
#define HyFileIO_h__

#include "Afx/HyStdAfx.h"
#include <fstream>

char *HyReadTextFile(const char *szFilePath, int *pLengthOut);

void HyReadTextFile(const char *szFilePath, std::string &sContentsOut);

void WriteTextFile(const char *szFilePath, const char *szContentBuffer);

bool HyFileExists(const std::string &sFilePath);

#endif /* HyFileIO_h__ */
