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
#include "Assets/Data/HyDataDraw.h"

#include <set>
using std::set;

// Forward declarations
class IHyRenderer;
class HyAtlasContainer;

class IHyData
{
	const HyType					m_eTYPE;
	const std::string				m_sPATH;

public:
	IHyData(HyType eInstType, const std::string &sPath);
	virtual ~IHyData(void);
	
	HyType GetInstType();
	const std::string &GetPath();

	virtual void SetRequiredAtlasIds(HyGfxData &gfxDataOut) = 0;
};

#endif /* __IHyData_h__ */
