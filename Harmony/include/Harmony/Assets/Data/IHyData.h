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
	const HyType					m_eTYPE;
	const std::string				m_sPATH;

public:
	IHyData(HyType eInstType, const std::string &sPath);
	virtual ~IHyData(void);
	
	HyType GetInstType();
	const std::string &GetPath();
};

#endif /* __IHyData_h__ */
