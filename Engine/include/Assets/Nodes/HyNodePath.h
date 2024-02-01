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
	std::string		m_sPath;
	uint32			m_uiHash;

public:
	HyNodePath(); // Invalid Path
	HyNodePath(const std::string &sPath); // Project Path
	HyNodePath(const std::string &sPrefix, const std::string &sName); // Project Path
	HyNodePath(uint32 uiAuxFirst, uint32 uiAuxSecond); // Auxiliary Path
	HyNodePath(const HyNodePath &copyRef);
	HyNodePath(HyNodePath &&donor);
	~HyNodePath();
	HyNodePath &operator=(const HyNodePath &rhs);

	bool operator==(const HyNodePath &rhs) const;
	bool operator!=(const HyNodePath &rhs) const;

	uint32 GetHash() const;
	std::string GetPath() const;
	std::string GetName() const;
	std::string GetPrefix() const;
	bool Set(const std::string &sPath); // Project Set
	bool Set(const std::string &sPrefix, const std::string &sName); // Project Set
	bool Set(uint32 uiAuxFirst, uint32 uiAuxSecond); // Auxiliary Set

	bool IsValid() const;
	bool IsAuxiliary() const;
};

#endif /* HyNodePath_h__ */
