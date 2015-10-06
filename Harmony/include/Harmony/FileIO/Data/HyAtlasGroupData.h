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

#include "FileIO/Data/IHyData.h"

#include <vector>
using std::vector;

// Forward declarations
class IHyRenderer;

class HyAtlasGroupData : public IHyData
{
	friend class IHyFileIO;

	static int32			sm_iWidth;
	static int32			sm_iHeight;
	static int32			sm_iNum8bitClrChannels;

	uint32					m_iGraphicsApiId;

	struct Texture
	{
		const uint32		uiID;
		const std::string	sPATH;

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

		struct Frame
		{
			HyRectangle		rSrcRect;
			bool			bRotated;
		};
		vector<Frame>		m_vFrames;

		uint32 GetId() const						{ return uiID; }
		const std::string &GetPath() const			{ return sPATH; }
	};
	vector<Texture>			m_vTextures;

public:
	HyAtlasGroupData(const std::string &sPath, HyRectangle *pSrcRects = NULL, uint32 uiNumRects = 0);
	~HyAtlasGroupData(void);

	static int32 GetWidth()							{ return sm_iWidth; }
	static int32 GetHeight() 						{ return sm_iHeight; }
	static int32 GetNumClrChannels() 				{ return sm_iNum8bitClrChannels; }

	void Upload(IHyRenderer &gfxApi);

	void DeletePixelData();

private:
	static void SetAtlasInfo(int32 iWidth, int32 iHeight, int32 iNum8bitClrChannels);
};

#endif /* __HyAtlas_h__ */
