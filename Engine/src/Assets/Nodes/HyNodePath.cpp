/**************************************************************************
 *	HyNodePath.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/

#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyNodePath.h"
#include "Utilities/Crc32.h"

HyNodePath::HyNodePath() :
	m_uiHash(0)
{
}

HyNodePath::HyNodePath(const std::string &sPrefix, const std::string &sName) :
	m_uiHash(0)
{
	Set(sPrefix, sName);
}

HyNodePath::HyNodePath(const HyNodePath &copyRef) :
	m_sPrefix(copyRef.m_sPrefix),
	m_sName(copyRef.m_sName),
	m_uiHash(copyRef.m_uiHash)
{
}

HyNodePath::HyNodePath(HyNodePath &&donor) :
	m_sPrefix(std::move(donor.m_sPrefix)),
	m_sName(std::move(donor.m_sName)),
	m_uiHash(donor.m_uiHash)
{
}

HyNodePath::~HyNodePath()
{
}

HyNodePath &HyNodePath::operator=(const HyNodePath &rhs)
{
	m_sPrefix = rhs.m_sPrefix;
	m_sName = rhs.m_sName;
	m_uiHash = rhs.m_uiHash;
	return *this;
}

bool HyNodePath::operator==(const HyNodePath &rhs) const
{
	// TODO: Create a 32bit hash of the path and compare that instead
	return m_sPrefix == rhs.m_sPrefix && m_sName == rhs.m_sName;
}

bool HyNodePath::operator!=(const HyNodePath &rhs) const
{
	return !(*this == rhs);
}

std::string HyNodePath::GetPath() const
{
	return m_sPrefix + "/" + m_sName;
}

std::string HyNodePath::GetName() const
{
	return m_sName;
}

std::string HyNodePath::GetPrefix() const
{
	return m_sPrefix;
}

void HyNodePath::Set(const std::string &sPrefix, const std::string &sName)
{
	m_sPrefix = sPrefix;
	m_sName = sName;

	// TODO:
	//m_uiHash = crc32_fast(&m_SamplersList[0], m_SamplersList.size() * sizeof(HyTextureHandle), 0);// m_uiSamplersCrc32);
}
