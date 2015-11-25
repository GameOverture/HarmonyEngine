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
	const HyInstanceType			m_eINSTTYPE;
	const std::string				m_sFILEPATH;

	int32							m_iRefCount;

	HyLoadState						m_eLoadState;

public:
	IHyData(HyDataType eDataType, HyInstanceType eInstType, const std::string &sPath) : m_eDATATYPE(eDataType),
																						m_eINSTTYPE(eInstType),
																						m_sFILEPATH(sPath),
																						m_eLoadState(HYLOADSTATE_Inactive),
																						m_iRefCount(0)
	{ }

	virtual ~IHyData(void)
	{ }

	HyDataType GetDataType()										{ return m_eDATATYPE; }
	HyInstanceType GetInstType()									{ return m_eINSTTYPE; }
	const std::string &GetPath()									{ return m_sFILEPATH; }

	virtual void SetLoadState(HyLoadState eState)					{ m_eLoadState = eState; }
	HyLoadState GetLoadState()										{ return m_eLoadState; }

	void IncRef()													{ m_iRefCount++; }
	bool DecRef()													{ m_iRefCount--; return m_iRefCount <= 0; }
	int32 GetRefCount()												{ return m_iRefCount; }
	
	// Only invoked on the Load thread
	virtual void DoFileLoad() = 0;
};

#endif /* __IHyData_h__ */
