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
#include "Diagnostics/Console/IHyConsole.h"
#include "Assets/HyAssets.h"

/*static*/ HyAssets *IHyLoadable::sm_pHyAssets = nullptr;

IHyLoadable::IHyLoadable(const HyNodePath &nodePath) :
	m_eLoadState(HYLOADSTATE_Inactive),
	m_pData(nullptr),
	m_NodePath(nodePath),
	m_uiState(0)
{
}

IHyLoadable::IHyLoadable(const IHyLoadable &copyRef) :
	m_eLoadState(HYLOADSTATE_Inactive),
	m_pData(copyRef.m_pData),
	m_NodePath(copyRef.m_NodePath),
	m_uiState(copyRef.m_uiState)
{
}

IHyLoadable::IHyLoadable(IHyLoadable &&donor) noexcept :
	m_eLoadState(HYLOADSTATE_Inactive),
	m_pData(std::move(donor.m_pData)),
	m_NodePath(std::move(donor.m_NodePath)),
	m_uiState(std::move(donor.m_uiState))
{
}

/*virtual*/ IHyLoadable::~IHyLoadable()
{
}

IHyLoadable &IHyLoadable::operator=(const IHyLoadable &rhs)
{
	if(m_NodePath != rhs.m_NodePath)
	{
		if(m_eLoadState != HYLOADSTATE_Inactive)
			Unload();

		m_NodePath = rhs.m_NodePath;
		m_pData = nullptr;			// Ensures virtual OnDataAcquired() is invoked when the below Load() is invoked
	}

	if(rhs.IsLoaded())
		Load();

	m_uiState = rhs.m_uiState;

	return *this;
}

IHyLoadable &IHyLoadable::operator=(IHyLoadable &&donor)
{
	if(m_NodePath != donor.m_NodePath)
	{
		if(m_eLoadState != HYLOADSTATE_Inactive)
			Unload();

		m_NodePath = std::move(donor.m_NodePath);
		m_pData = nullptr;			// Ensures virtual OnDataAcquired() is invoked when the below Load() is invoked
	}

	if(donor.IsLoaded())
	{
		Load();
		donor.Unload();
	}

	m_uiState = std::move(donor.m_uiState);

	return *this;
}

const HyNodePath &IHyLoadable::GetPath() const
{
	return m_NodePath;
}

std::string IHyLoadable::GetName() const
{
	return m_NodePath.GetName();
}

std::string IHyLoadable::GetPrefix() const
{
	return m_NodePath.GetPrefix();
}

void IHyLoadable::_Reinitialize(const HyNodePath &nodePathRef)
{
	if(m_NodePath == nodePathRef)
		return;

	bool bWasLoaded = IsLoaded() || m_eLoadState == HYLOADSTATE_Queued;
	if(bWasLoaded)
		Unload();

	m_NodePath = nodePathRef;
	m_pData = nullptr;
	m_uiState = 0;

	if(bWasLoaded)
		Load();
}

/*virtual*/ uint32 IHyLoadable::GetState() const
{
	return m_uiState;
}

/*virtual*/ bool IHyLoadable::SetState(uint32 uiStateIndex)
{
	if(_LoadableGetType() != HYTYPE_Entity &&
	   (AcquireData() == nullptr || uiStateIndex >= UncheckedGetData()->GetNumStates()))
	{
		if(UncheckedGetData() == nullptr)
			HyLogWarning(m_NodePath.GetPath() << " wants to set state index of '" << uiStateIndex << "' when data is null");
		else
			HyLogWarning(m_NodePath.GetPath() << " wants to set state index of '" << uiStateIndex << "' when total number of states is '" << UncheckedGetData()->GetNumStates() << "'");
		return false;
	}

	m_uiState = uiStateIndex;
	return true;
}

/*virtual*/ uint32 IHyLoadable::GetNumStates()
{
	if(AcquireData() == nullptr)
	{
		HyLogDebug("IHyLoadable::GetNumStates invoked on null data");
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

HyLoadState IHyLoadable::GetLoadState() const
{
	return m_eLoadState;
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

const IHyNodeData *IHyLoadable::UncheckedGetData() const
{
	return m_pData;
}
