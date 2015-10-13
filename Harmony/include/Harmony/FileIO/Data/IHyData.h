/**************************************************************************
 *	IHyData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyData_h__
#define __IHyData_h__

#include "Afx/HyStdAfx.h"

#include "FileIO/HyAtlasManager.h"

#include <set>
using std::set;

// Forward declarations
class IHyRenderer;
class HyAtlasManager;

class IHyData
{
	struct AtlasManager
	{
		static std::string		sm_sAtlasDirPath;

		int32					m_iWidth;
		int32					m_iHeight;
		int32					m_iNum8bitClrChannels;

		struct AtlasGroup
		{
			const int32					m_iLOADGROUPID;
			uint32						m_uiGfxApiHandle;

			struct Atlas
			{
				const uint32			m_uiTEXTUREID;
				unsigned char *			m_pPixelData;

				HyRectangle<int32> *	m_pFrames;
				uint32					m_uiNumFrames;
			};
			HyAtlas *					m_pAtlases;
			uint32						m_uiNumAtlases;

			HyLoadState					m_eLoadState;
			set<IHyData *>				m_AssociatedDataSet;
		};
		AtlasGroup *			m_pAtlasGroups;
		uint32					m_uiNumAtlasGroups;
	};
	static AtlasManager				sm_AtlasManager;

	const HyInstanceType			m_eTYPE;
	const std::string				m_sFILEPATH;

	HyLoadState						m_eLoadState;

	int32							m_iRefCount;


	std::set<HyAtlasGroup *>		m_AssociatedAtlasRefs;

public:
	IHyData(HyInstanceType eDataType, const std::string &sPath) :	m_eTYPE(eDataType),
																	m_sFILEPATH(sPath),
																	m_eLoadState(HYLOADSTATE_Inactive),
																	m_iRefCount(0)
	{ }

	virtual ~IHyData(void)
	{ }

	HyInstanceType GetType()										{ return m_eTYPE; }
	const std::string &GetPath()									{ return m_sFILEPATH; }

	void SetLoadState(HyLoadState eState)
	{
		m_eLoadState = eState;

		if(m_eLoadState == HYLOADSTATE_Queued)
		{
			for(std::set<HyAtlasGroup *>::iterator iter = m_AssociatedAtlasRefs.begin(); iter != m_AssociatedAtlasRefs.end(); ++iter)
				(*iter)->Request(this);
		}
		if(m_eLoadState == HYLOADSTATE_Discarded)
		{
			for(std::set<HyAtlasGroup *>::iterator iter = m_AssociatedAtlasRefs.begin(); iter != m_AssociatedAtlasRefs.end(); ++iter)
				(*iter)->Relinquish(this);
		}
		else if(m_eLoadState == HYLOADSTATE_ReloadGfx)
		{

		}
	}
	HyLoadState GetLoadState()										{ return m_eLoadState; }
	const std::set<HyAtlasGroup *> &GetAssociatedAtlases()			{ return m_AssociatedAtlasRefs; }

	void IncRef()													{ m_iRefCount++; }
	bool DecRef()													{ m_iRefCount--; return m_iRefCount <= 0; }
	int32 GetRefCount()												{ return m_iRefCount; }

	void RequestTexture(uint32 uiTextureId);
	
	// Only invoked on the Load thread
	virtual void DoFileLoad(HyAtlasManager &atlasManagerRef) = 0;

	// Only invoked on the Render thread
};

#endif /* __IHyData_h__ */
