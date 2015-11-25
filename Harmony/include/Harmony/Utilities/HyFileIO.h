/**************************************************************************
*	HyFileIO.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyFileIO_h__
#define __HyFileIO_h__

#include <stdio.h>
#include <fstream>

char *HyReadTextFile(const char *szFilePath, int *iLength);

std::string HyReadTextFile(const char *szFilePath);

bool HyFileExists(const std::string &sFilePath);

#endif /* __HyFileIO_h__ */
