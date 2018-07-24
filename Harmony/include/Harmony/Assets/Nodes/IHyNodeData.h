/**************************************************************************
 *	IHyNodeData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyNodeData_h__
#define IHyNodeData_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Loadables/HyAtlasIndices.h"

// Forward declarations
class IHyRenderer;

class IHyNodeData
{
	const std::string				m_sPATH;

protected:
	HyAtlasIndices					m_RequiredAtlasIndices;

public:
	IHyNodeData(const std::string &sPath);
	virtual ~IHyNodeData(void);
	
	const std::string &GetPath() const;
	const HyAtlasIndices &GetRequiredAtlasIndices() const;
};

#endif /* IHyNodeData_h__ */
