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
	uint32			m_uiHash1;
	uint32			m_uiHash2;

public:
	HyNodePath(); // Invalid Path
	HyNodePath(const char *szPath); // Project Path
	HyNodePath(const char *szPrefix, const char *szName); // Project Path
	HyNodePath(uint32 uiChecksum, uint32 uiBankId); // Auxiliary Path
	HyNodePath(const HyNodePath &copyRef);
	HyNodePath(HyNodePath &&donor);
	~HyNodePath();
	HyNodePath &operator=(const HyNodePath &rhs);

	bool operator==(const HyNodePath &rhs) const;
	bool operator!=(const HyNodePath &rhs) const;

	uint32 GetHash1() const;	// IF PROJECT: contains m_sPath's hash	IF AUXILIARY: contains bank ID (or 0 for external files)
	uint32 GetHash2() const;	// IF PROJECT: contains 0				IF AUXILIARY: contains checksum
	std::string GetPath() const;
	std::string GetName() const;
	std::string GetPrefix() const;
	bool Set(const char *szPath); // Project Set
	bool Set(const char *szPrefix, const char *szName); // Project Set
	bool Set(uint32 uiChecksum, uint32 uiBankId); // Auxiliary Set

	bool IsValid() const;
	bool IsAuxiliary() const;
};

#endif /* HyNodePath_h__ */
