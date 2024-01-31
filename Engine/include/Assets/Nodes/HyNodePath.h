/**************************************************************************
 *	HyNodePath.h
 *
 *	Harmony Engine
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyNodePath_h__
#define HyNodePath_h__

#include "Afx/HyStdAfx.h"

class HyNodePath
{
	// TODO: Combine these into one std string 'path'
	std::string		m_sPrefix;
	std::string		m_sName;

	uint32			m_uiHash;

public:
	HyNodePath();
	HyNodePath(const std::string &sPrefix, const std::string &sName);
	HyNodePath(const HyNodePath &copyRef);
	HyNodePath(HyNodePath &&donor);
	~HyNodePath();
	HyNodePath &operator=(const HyNodePath &rhs);

	bool operator==(const HyNodePath &rhs) const;
	bool operator!=(const HyNodePath &rhs) const;

	std::string GetPath() const;
	std::string GetName() const;
	std::string GetPrefix() const;
	void Set(const std::string &sPrefix, const std::string &sName);
};

#endif /* HyNodePath_h__ */
