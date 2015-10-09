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

#include <vector>
using std::vector;

// Forward declarations
class IHyRenderer;
class HyAtlasManager;

class IHyData
{
protected:
	const HyInstanceType			m_eTYPE;
	const std::string				m_sFILEPATH;
	
	HyLoadState						m_eLoadState;
	int32							m_iRefCount;

public:
	IHyData(HyInstanceType eDataType, const std::string &sPath) :	m_eTYPE(eDataType),
																	m_sFILEPATH(sPath),
																	m_eLoadState(HYLOADSTATE_Inactive),
																	m_iRefCount(0)
	{ }

	virtual ~IHyData(void)
	{ }

	HyInstanceType GetType()							{ return m_eTYPE; }
	const std::string &GetPath()						{ return m_sFILEPATH; }

	void SetLoadState(HyLoadState eState)				{ m_eLoadState = eState; }
	HyLoadState GetLoadState()							{ return m_eLoadState; }

	void IncRef()										{ m_iRefCount++; }
	bool DecRef()										{ m_iRefCount--; return m_iRefCount <= 0; }
	int32 GetRefCount()									{ return m_iRefCount; }
	
	// Only invoked on the Load thread
	virtual void DoFileLoad(HyAtlasManager &atlasManagerRef) = 0;

	// Only invoked on the Render thread
	virtual void GetAssociatedAtlases(std::set<HyAtlasGroup> &atlasGrp) = 0;
};

#endif /* __IHyData_h__ */
