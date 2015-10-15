/**************************************************************************
 *	HyAtlasManager.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAtlasManager_h__
#define __HyAtlasManager_h__

#include "Afx/HyStdAfx.h"

#include "Threading/Threading.h"

#include "Utilities/HyMath.h"
#include "Utilities/jsonxx.h"

#include <set>
using std::set;

class HyAtlasGroup;
class HyAtlas;

class IHyData;
class IHyRenderer;

//////////////////////////////////////////////////////////////////////////
class HyAtlasManager
{
	std::string				m_sAtlasDirPath;

	HyAtlasGroup *			m_pAtlasGroups;
	uint32					m_uiNumAtlasGroups;

public:
	HyAtlasManager(std::string sAtlasDataDir);
	~HyAtlasManager();

	HyAtlasGroup *RequestTexture(uint32 uiTextureId);
	std::string GetTexturePath(uint32 uiTextureId);
};

//////////////////////////////////////////////////////////////////////////
class HyAtlasGroup
{
	friend class HyAtlasManager;

	HyAtlasManager &			m_ManagerRef;

	const uint32				m_uiLOADGROUPID;
	const uint32				m_uiWIDTH;
	const uint32				m_uiHEIGHT;
	const uint32				m_uiNUM_8BIT_CHANNELS;


	HyAtlas *					m_pAtlases;
	const uint32				m_uiNUM_ATLASES;

	uint32						m_uiGfxApiHandle;

	set<IHyData *>				m_AssociatedDataSet;
	BasicSection				m_csDataRefs;
	BasicSection				m_csTextures;

public:
	HyAtlasGroup(HyAtlasManager &managerRef, uint32 uiLoadGroupId, uint32 uiWidth, uint32 uiHeight, uint32 uiNumClrChannels, jsonxx::Array &texturesArrayRef);
	~HyAtlasGroup();

	uint32 GetGfxApiHandle();
	uint32 GetNumColorChannels();
	uint32 GetWidth();
	uint32 GetHeight();

	bool ContainsTexture(uint32 uiTextureId);
	void Load();

	void Assign(IHyData *pData);
	void Relinquish(IHyData *pData);

	void OnRenderThread(IHyRenderer &rendererRef);
};

//////////////////////////////////////////////////////////////////////////
class HyAtlas
{
	const uint32			m_uiTEXTUREID;

	// The return value from the 'stb_image' loader is an 'unsigned char *' which points
	// to the pixel data. The pixel data consists of *y scanlines of *x pixels,
	// with each pixel consisting of N interleaved 8-bit components; the first
	// pixel pointed to is top-left-most in the image. There is no padding between
	// image scanlines or between pixels, regardless of format. The number of
	// components N is 'req_comp' if req_comp is non-zero, or *comp otherwise.
	// If req_comp is non-zero, *comp has the number of components that _would_
	// have been output otherwise. E.g. if you set req_comp to 4, you will always
	// get RGBA output, but you can check *comp to easily see if it's opaque.
	unsigned char *			m_pPixelData;

	HyRectangle<int32> *	m_pFrames;
	uint32					m_uiNumFrames;

public:
	HyAtlas(uint32 uiTextureId, jsonxx::Array &srcFramesArrayRef);
	~HyAtlas();

	uint32 GetId();

	void Load(const char *szFilePath);

	unsigned char *GetPixelData();
	void DeletePixelData();
};

#endif __HyAtlasManager_h__
