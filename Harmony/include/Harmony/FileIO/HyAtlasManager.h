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

#include <queue>
using std::queue;

class HyAtlasGroup;
class HyAtlas;

//////////////////////////////////////////////////////////////////////////
class HyAtlasManager
{
	static std::string	sm_sAtlasDirPath;

	int32					m_iWidth;
	int32					m_iHeight;
	int32					m_iNum8bitClrChannels;

	HyAtlasGroup *			m_pAtlasGroups;
	uint32					m_uiNumAtlasGroups;

	BasicSection			m_cs;
	vector<IHyData *>		m_vDataWaitingForAtlasUpload;
	queue<HyAtlasGroup *>	m_AtlasesWaitingForAtlasUpload;

public:
	HyAtlasManager(const char *szDataDirPath);
	~HyAtlasManager();

	bool RequestTexture(IHyData *pData, uint32 uiTextureId);
	void RelinquishTexture(IHyData *pData, uint32 uiTextureId);

	bool IsDataWaitingForUpload(IHyData *pData);
	void GetAtlasesThatNeedUpload(queue<HyAtlasGroup *> &vAtlasesThatNeedUpload);
	
	static std::string GetTexturePath(uint32 uiTextureId);
};

//////////////////////////////////////////////////////////////////////////
class HyAtlasGroup
{
	const int32					m_iLOADGROUPID;
	uint32						m_uiGraphicsApiId;

	HyAtlas *					m_pAtlases;
	uint32						m_uiNumAtlases;

	HyLoadState					m_eLoadState;
	vector<IHyData *>			m_vAssociatedData;

	BasicSection				m_cs;

public:
	HyAtlasGroup(int32 iLoadGroupId, jsonxx::Array &texturesArrayRef);
	~HyAtlasGroup();

	bool ContainsTexture(uint32 uiTextureId);

	// Returns 'true' if texture was just loaded
	bool Request(IHyData *pData);

	bool IsUploadNeeded();

	//void Upload(IHyRenderer &gfxApi);

	//void DeletePixelData();
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

	void Load();
};

#endif __HyAtlasManager_h__
