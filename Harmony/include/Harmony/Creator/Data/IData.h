/**************************************************************************
 *	IData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IData_h__
#define __IData_h__

#include "Utilities/HyStrManip.h"
#include "Utilities/Animation/HyAnimVec2.h"
#include "HyFactory.h"

#include "FileIO/HyFileIO.h"

// Forward declarations
class IHyRenderer;

class IData
{
protected:
	const HyInstanceType			m_keDataType;
	const std::string				m_ksPath;
	
	HyLoadState						m_eLoadState;
	int32							m_iRefCount;

public:
	IData(HyInstanceType eDataType, const std::string &sPath) :	m_keDataType(eDataType),
																m_ksPath(sPath),
																m_eLoadState(HYLOADSTATE_Inactive),
																m_iRefCount(0)
	{ }

	virtual ~IData(void)
	{ }

	HyInstanceType GetType()							{ return m_keDataType; }

	const std::string &GetPath()						{ return m_ksPath; }

	void SetLoadState(HyLoadState eState)				{ m_eLoadState = eState; }
	HyLoadState GetLoadState()							{ return m_eLoadState; }

	void IncRef()										{ m_iRefCount++; }
	bool DecRef()										{ m_iRefCount--; return m_iRefCount <= 0; }
	int32 GetRefCount()									{ return m_iRefCount; }

	bool IsMatch(const std::string &sPath)
	{
		return sPath == m_ksPath;
	}
	
	// Only invoked on the Load thread
	virtual void DoFileLoad() = 0;

	// Only invoked on the Render thread
	virtual void OnGfxLoad(IHyRenderer &gfxApi) = 0;
	virtual void OnGfxRemove(IHyRenderer &gfxApi) = 0;
};

#endif /* __IData_h__ */
