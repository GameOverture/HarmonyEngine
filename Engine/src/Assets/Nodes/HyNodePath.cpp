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
#include "Utilities/HyIO.h"

#define HYNODEPATH_PROJECT_HASH2 0xFFFFFFFF

HyNodePath::HyNodePath() :
	m_uiHash1(0),
	m_uiHash2(0)
{
}

HyNodePath::HyNodePath(const char *szPath) :
	m_uiHash1(0),
	m_uiHash2(0)
{
	Set(szPath);
}

HyNodePath::HyNodePath(const char *szPrefix, const char *szName) :
	m_uiHash1(0),
	m_uiHash2(0)
{
	Set(szPrefix, szName);
}

HyNodePath::HyNodePath(uint32 uiChecksum, uint32 uiBankId) :
	m_uiHash1(0),
	m_uiHash2(0)
{
	Set(uiChecksum, uiBankId);
}

HyNodePath::HyNodePath(const HyNodePath &copyRef) :
	m_sPath(copyRef.m_sPath),
	m_uiHash1(copyRef.m_uiHash1),
	m_uiHash2(copyRef.m_uiHash2)
{
}

HyNodePath::HyNodePath(HyNodePath &&donor) :
	m_sPath(std::move(donor.m_sPath)),
	m_uiHash1(donor.m_uiHash1),
	m_uiHash2(donor.m_uiHash2)
{
}

HyNodePath::~HyNodePath()
{
}

HyNodePath &HyNodePath::operator=(const HyNodePath &rhs)
{
	m_sPath = rhs.m_sPath;
	m_uiHash1 = rhs.m_uiHash1;
	m_uiHash2 = rhs.m_uiHash2;
	return *this;
}

bool HyNodePath::operator==(const HyNodePath &rhs) const
{
	return m_uiHash1 == rhs.m_uiHash1 && m_uiHash2 == rhs.m_uiHash2;
}

bool HyNodePath::operator!=(const HyNodePath &rhs) const
{
	return !(*this == rhs);
}

uint32 HyNodePath::GetHash1() const
{
	return m_uiHash1;
}

uint32 HyNodePath::GetHash2() const
{
	return m_uiHash2;
}

std::string HyNodePath::GetPath() const
{
	return m_sPath;
}

std::string HyNodePath::GetName() const
{
	if(m_sPath.find('/') == std::string::npos)
		return m_sPath;
	else
		return m_sPath.substr(m_sPath.find_last_of('/') + 1);
}

std::string HyNodePath::GetPrefix() const
{
	if(m_sPath.find('/') == std::string::npos)
		return "";
	else
		return m_sPath.substr(0, m_sPath.find_last_of('/'));
}

bool HyNodePath::Set(const char *szPath)
{
	m_sPath = HyIO::CleanPath(szPath ? szPath : "");

	if(m_sPath.empty())
	{
		m_uiHash1 = m_uiHash2 = 0;
		return false;
	}
	else
	{
		std::string sHash = m_sPath; // Preserve case in 'm_sPath', but hash is case-insensitive
		HyIO::MakeLowercase(sHash);
		m_uiHash1 = crc32_fast(sHash.c_str(), sHash.size(), 0);
		m_uiHash2 = HYNODEPATH_PROJECT_HASH2;
	}

	return true;
}

bool HyNodePath::Set(const char *szPrefix, const char *szName)
{
	std::string sPrefix(szPrefix ? szPrefix : "");
	if(sPrefix.empty() == false)
		m_sPath = sPrefix + "/";

	std::string sName(szName ? szName : "");
	m_sPath += sName;

	return Set(m_sPath.c_str());
}

bool HyNodePath::Set(uint32 uiChecksum, uint32 uiBankId)
{
	m_sPath.clear();
	m_uiHash1 = uiChecksum;
	m_uiHash2 = uiBankId;

	return m_uiHash1 != 0;
}

void HyNodePath::Clear()
{
	m_sPath.clear();
	m_uiHash1 = m_uiHash2 = 0;
}

bool HyNodePath::IsValid() const
{
	return m_uiHash1 != 0 || m_uiHash2 != 0;
}

bool HyNodePath::IsAuxiliary() const
{
	return IsValid() && m_uiHash2 != HYNODEPATH_PROJECT_HASH2;
}
