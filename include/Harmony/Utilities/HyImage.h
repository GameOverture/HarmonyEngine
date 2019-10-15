/**************************************************************************
*	HyImage.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyImage_h__
#define HyImage_h__

#include "Afx/HyStdAfx.h"
#include "soil2/SOIL2.h"

bool HySaveImage_DTX5(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData);
bool HySaveImage_DTX1(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData);

#endif /* HyImage_h__ */
