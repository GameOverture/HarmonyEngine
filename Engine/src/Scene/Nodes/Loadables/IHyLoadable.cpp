/**************************************************************************
*	IHyLoadable.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Assets/HyAssets.h"

/*static*/ HyAssets *IHyLoadable::sm_pHyAssets = nullptr;

IHyLoadable::IHyLoadable() :
	m_eLoadState(HYLOADSTATE_Inactive),
	m_pData(nullptr),
	m_sName(""),
	m_sPrefix(""),
	m_uiState(0)
{
}

IHyLoadable::IHyLoadable(const IHyLoadable &copyRef) :
	m_eLoadState(HYLOADSTATE_Inactive),
	m_pData(copyRef.m_pData),
	m_sName(copyRef.m_sName),
	m_sPrefix(copyRef.m_sPrefix),
	m_uiState(copyRef.m_uiState)
{
}

IHyLoadable::IHyLoadable(IHyLoadable &&donor) noexcept :
	m_eLoadState(HYLOADSTATE_Inactive),
	m_pData(std::move(donor.m_pData)),
	m_sName(std::move(donor.m_sName)),
	m_sPrefix(std::move(donor.m_sPrefix)),
	m_uiState(std::move(donor.m_uiState))
{
}

/*virtual*/ IHyLoadable::~IHyLoadable()
{
}

IHyLoadable &IHyLoadable::operator=(const IHyLoadable &rhs)
{
	if(m_sPrefix != rhs.m_sPrefix || m_sName != rhs.m_sName)
	{
		if(m_eLoadState != HYLOADSTATE_Inactive)
			Unload();

		m_sPrefix = rhs.m_sPrefix;
		m_sName = rhs.m_sName;
		m_pData = nullptr;			// Ensures virtual OnDataAcquired() is invoked when the below Load() is invoked
	}

	if(rhs.IsLoaded())
		Load();

	return *this;
}

IHyLoadable &IHyLoadable::operator=(IHyLoadable &&donor)
{
	if(m_sPrefix != donor.m_sPrefix || m_sName != donor.m_sName)
	{
		if(m_eLoadState != HYLOADSTATE_Inactive)
			Unload();

		m_sPrefix = std::move(donor.m_sPrefix);
		m_sName = std::move(donor.m_sName);
		m_pData = nullptr;			// Ensures virtual OnDataAcquired() is invoked when the below Load() is invoked
	}

	if(donor.IsLoaded())
	{
		Load();
		donor.Unload();
	}

	return *this;
}

const std::string &IHyLoadable::GetName() const
{
	return m_sName;
}

const std::string &IHyLoadable::GetPrefix() const
{
	return m_sPrefix;
}

void IHyLoadable::_Reinitialize(std::string sPrefix, std::string sName)
{
	if(m_sPrefix == sPrefix && m_sName == sName)
		return;

	bool bWasLoaded = IsLoaded();
	if(bWasLoaded)
		Unload();

	m_sPrefix = sPrefix;
	m_sName = sName;

	if(bWasLoaded)
		Load();
}

uint32 IHyLoadable::GetState() const
{
	return m_uiState;
}

/*virtual*/ void IHyLoadable::SetState(uint32 uiStateIndex)
{
	m_uiState = uiStateIndex;
}

uint32 IHyLoadable::GetNumStates()
{
	if(AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimGetNumStates invoked on null data");
		return 0;
	}
	
	return UncheckedGetData()->GetNumStates();
}

const IHyNodeData *IHyLoadable::AcquireData()
{
	if(m_pData == nullptr)
	{
		HyAssert(sm_pHyAssets != nullptr, "AcquireData was called before the engine has initialized HyAssets");

		sm_pHyAssets->AcquireNodeData(this, m_pData);
		if(m_pData)
			OnDataAcquired();
	}

	return m_pData;
}

bool IHyLoadable::IsLoaded() const
{
	return m_eLoadState == HYLOADSTATE_Loaded;
}

/*virtual*/ void IHyLoadable::Load()
{
	HyAssert(sm_pHyAssets, "IHyLoadable::Load was invoked before engine has been initialized");
	sm_pHyAssets->LoadNodeData(this);
}

/*virtual*/ void IHyLoadable::Unload()
{
#if defined(HY_PLATFORM_GUI) // Fix order of deletions when shutting down Editor, else keep this
	if(sm_pHyAssets == nullptr)
		return;
#endif

	HyAssert(sm_pHyAssets, "IHyLoadable::Unload was invoked before engine has been initialized");
	sm_pHyAssets->RemoveNodeData(this);
}

const IHyNodeData *IHyLoadable::UncheckedGetData()
{
	return m_pData;
}
