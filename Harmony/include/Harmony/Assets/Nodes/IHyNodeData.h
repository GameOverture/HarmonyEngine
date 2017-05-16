/**************************************************************************
 *	IHyNodeData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyNodeData_h__
#define IHyNodeData_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Loadables/HyAtlasIndices.h"

// Forward declarations
class IHyRenderer;
class HyAtlasContainer;

class IHyNodeData
{
	const HyType					m_eTYPE;
	const std::string				m_sPATH;

protected:
	HyAtlasIndices					m_RequiredAtlasIndices;

public:
	IHyNodeData(HyType eInstType, const std::string &sPath);
	virtual ~IHyNodeData(void);
	
	HyType GetInstType();
	const std::string &GetPath();

	const HyAtlasIndices &GetRequiredAtlasIndices();
};

#endif /* IHyNodeData_h__ */
