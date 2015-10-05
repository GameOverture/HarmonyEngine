/**************************************************************************
 *	HyAtlas.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAtlas_h__
#define __HyAtlas_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/HyMath.h"

#include <vector>
using std::vector;

// Forward declarations
class IHyRenderer;

class HyAtlas
{
	uint32				m_uiId;

	const std::string	m_ksPath;
	int32				m_iWidth;
	int32				m_iHeight;
	int32				m_iNum8bitClrChannels;

	// The return value from the 'stb_image' loader is an 'unsigned char *' which points
	// to the pixel data. The pixel data consists of *y scanlines of *x pixels,
	// with each pixel consisting of N interleaved 8-bit components; the first
	// pixel pointed to is top-left-most in the image. There is no padding between
	// image scanlines or between pixels, regardless of format. The number of
	// components N is 'req_comp' if req_comp is non-zero, or *comp otherwise.
	// If req_comp is non-zero, *comp has the number of components that _would_
	// have been output otherwise. E.g. if you set req_comp to 4, you will always
	// get RGBA output, but you can check *comp to easily see if it's opaque.
	unsigned char *		m_pPixelData;

	HyRectangle *		m_pSrcRects;
	uint32				m_uiNumRects;

public:
	HyAtlas(const std::string &sPath, HyRectangle *pSrcRects = NULL, uint32 uiNumRects = 0);
	~HyAtlas(void);

	int32 GetWidth() const				{ return m_iWidth; }
	int32 GetHeight() const				{ return m_iHeight; }
	int32 GetNumClrChannels() const		{ return m_iNum8bitClrChannels; }

	const std::string &GetPath() const	{ return m_ksPath; }
	uint32 GetId() const				{ return m_uiId; }

	void Upload(IHyRenderer &gfxApi);

	void DeletePixelData();
};

#endif /* __HyAtlas_h__ */
