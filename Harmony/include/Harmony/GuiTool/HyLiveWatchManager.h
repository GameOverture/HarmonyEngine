/**************************************************************************
 *	HyLiveWatchManager.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyLiveWatchManager_h__
#define __HyLiveWatchManager_h__

#include "Afx/HyStdAfx.h"

#include <vector>
using std::vector;

class HyLiveWatch
{
	static uint32	sm_uiGuidCounter;

	uint32			m_uiGuid;
	std::string		m_sName;

	struct tGroup
	{
		// Keeps track of 4 byte values
		struct tVar
		{
			void *pValuePtr;
			uint32 uiCachedRawValue;

			tVar(void *pVar) :	pValuePtr(pVar),
								uiCachedRawValue(*reinterpret_cast<uint32 *>(pValuePtr))
			{
			}

			bool IsDirty()
			{
				if(*reinterpret_cast<uint32 *>(pValuePtr) != uiCachedRawValue)
				{
					uiCachedRawValue = *reinterpret_cast<uint32 *>(pValuePtr);
					return true;
				}
				return false;
			}
		};
		vector<tVar>	m_vVariables;
	};
	vector<tGroup>	m_vGroups;

public:
	HyLiveWatch();

	void AddWatch(const char *szGroupName, int32 *pVariable);
};

class HyLiveWatchManager
{
	static HyLiveWatchManager *		sm_pInstance;

	vector<HyLiveWatch>				m_vPages;

	HyLiveWatchManager(void);

public:
	~HyLiveWatchManager(void);

	HyLiveWatchManager *GetInstance();

	static HyLiveWatch *CreateLiveWatchPage(std::string sName);
	static HyLiveWatch *GetLiveWatchPage(std::string sName);
};

#endif /* __HyLiveWatchManager_h__ */
