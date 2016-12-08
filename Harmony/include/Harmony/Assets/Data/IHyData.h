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

#include <set>
using std::set;

// Forward declarations
class IHyRenderer;
class HyTextures;

class IHyData
{
	const HyDataType				m_eDATATYPE;
	const HyType					m_eINSTTYPE;
	const std::string				m_sFILEPATH;

	int32							m_iRefCount;

	HyLoadState						m_eLoadState;

public:
	IHyData(HyDataType eDataType, HyType eInstType, const std::string &sPath);
	virtual ~IHyData(void);
	
	HyDataType GetDataType();
	HyType GetInstType();
	const std::string &GetPath();

	virtual void SetLoadState(HyLoadState eState);
	HyLoadState GetLoadState();

	void IncRef();
	bool DecRef();
	int32 GetRefCount();

	virtual void OnLoadThread();
	
	// Only invoked on the Load thread
	virtual void DoFileLoad() = 0;
};

#endif /* __IHyData_h__ */
