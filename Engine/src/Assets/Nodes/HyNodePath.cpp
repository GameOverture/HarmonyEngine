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

#define HYNODEPATH_INVALID_HASH 0xFFFFFFFF
#define HYNODEPATH_AUX_HASH 0

HyNodePath::HyNodePath() :
	m_uiHash(HYNODEPATH_INVALID_HASH)
{
}

HyNodePath::HyNodePath(const std::string &sPath) :
	m_uiHash(HYNODEPATH_INVALID_HASH) // This should become valid in Set()
{
	Set(sPath);
}

HyNodePath::HyNodePath(const std::string &sPrefix, const std::string &sName) :
	m_uiHash(HYNODEPATH_INVALID_HASH) // This should become valid in Set()
{
	Set(sPrefix, sName);
}

HyNodePath::HyNodePath(uint32 uiAuxFirst, uint32 uiAuxSecond) :
	m_uiHash(HYNODEPATH_INVALID_HASH)
{
	Set(uiAuxFirst, uiAuxSecond);
}

HyNodePath::HyNodePath(const HyNodePath &copyRef) :
	m_sPath(copyRef.m_sPath),
	m_uiHash(copyRef.m_uiHash)
{
}

HyNodePath::HyNodePath(HyNodePath &&donor) :
	m_sPath(std::move(donor.m_sPath)),
	m_uiHash(donor.m_uiHash)
{
}

HyNodePath::~HyNodePath()
{
}

HyNodePath &HyNodePath::operator=(const HyNodePath &rhs)
{
	m_sPath = rhs.m_sPath;
	m_uiHash = rhs.m_uiHash;
	return *this;
}

bool HyNodePath::operator==(const HyNodePath &rhs) const
{
	return m_uiHash == rhs.m_uiHash;
}

bool HyNodePath::operator!=(const HyNodePath &rhs) const
{
	return !(*this == rhs);
}

uint32 HyNodePath::GetHash() const
{
	return m_uiHash;
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

bool HyNodePath::Set(const std::string &sPath)
{
	m_sPath = HyIO::CleanPath(sPath);

	if(m_sPath.empty())
	{
		m_uiHash = HYNODEPATH_INVALID_HASH;
		return false;
	}
	else
	{
		std::string sHash = m_sPath; // Preserve case in 'm_sPath', but hash is case-insensitive
		HyIO::MakeLowercase(sHash);
		m_uiHash = crc32_fast(sHash.c_str(), sHash.size(), 0);
	}

	return true;
}

bool HyNodePath::Set(const std::string &sPrefix, const std::string &sName)
{
	if(sPrefix.empty() == false)
		m_sPath = sPrefix + "/";

	m_sPath += sName;

	return Set(m_sPath);
}

bool HyNodePath::Set(uint32 uiAuxFirst, uint32 uiAuxSecond)
{
	if(uiAuxFirst == 0 && uiAuxSecond == 0)
	{
		m_sPath.clear();
		m_uiHash = HYNODEPATH_INVALID_HASH;
		return false;
	}

	m_sPath = std::to_string(uiAuxFirst) + "/" + std::to_string(uiAuxSecond);
	m_uiHash = HYNODEPATH_AUX_HASH;
	
	return true;
}

bool HyNodePath::IsValid() const
{
	return m_uiHash != HYNODEPATH_INVALID_HASH;
}

bool HyNodePath::IsAuxiliary() const
{
	return m_uiHash == HYNODEPATH_AUX_HASH;
}
