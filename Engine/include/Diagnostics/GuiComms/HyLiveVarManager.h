/**************************************************************************
 *	HyLiveVarManager.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyLiveVarManager_h__
#define HyLiveVarManager_h__

#include "Afx/HyStdAfx.h"

//class HyLiveVar
//{
//	static uint32	sm_uiGuidCounter;
//
//	uint32			m_uiGuid;
//	std::string		m_sName;
//
//	struct tGroup
//	{
//		// Keeps track of 4 byte values
//		struct tVar
//		{
//			void *pValuePtr;
//			uint32 uiCachedRawValue;
//
//			tVar(void *pVar) :	pValuePtr(pVar),
//								uiCachedRawValue(*reinterpret_cast<uint32 *>(pValuePtr))
//			{
//			}
//
//			bool IsDirty()
//			{
//				if(*reinterpret_cast<uint32 *>(pValuePtr) != uiCachedRawValue)
//				{
//					uiCachedRawValue = *reinterpret_cast<uint32 *>(pValuePtr);
//					return true;
//				}
//				return false;
//			}
//		};
//		std::vector<tVar>	m_VariableList;
//	};
//	std::vector<tGroup>	m_GroupList;
//
//public:
//	HyLiveVar();
//
//	void AddWatch(const char *szGroupName, int32 *pVariable);
//};

//class HyLiveVarManager
//{
//	static HyLiveVarManager *		sm_pInstance;
//
//	std::vector<HyLiveVar>			m_PageList;
//
//	HyLiveVarManager(void);
//
//public:
//	~HyLiveVarManager(void);
//
//	HyLiveVarManager *GetInstance();
//
//	static HyLiveVar *CreateLiveWatchPage(std::string sName);
//	static HyLiveVar *GetLiveWatchPage(std::string sName);
//};

#endif /* HyLiveVarManager_h__ */
