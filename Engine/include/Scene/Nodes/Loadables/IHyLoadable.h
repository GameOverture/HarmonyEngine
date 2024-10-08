/**************************************************************************
*	IHyLoadable.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLoadable_h__
#define IHyLoadable_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/Nodes/HyNodePath.h"

class HyEntity2d;
class HyEntity3d;
class HyAssets;

class IHyLoadable
{
	friend class HyAssets;
	friend class HyDiagnostics;

protected:
	static HyAssets *				sm_pHyAssets;
	
	HyLoadState						m_eLoadState;
	const IHyNodeData *				m_pData;
	HyNodePath						m_NodePath;

	uint32							m_uiState;	// All 'Loadable' type nodes utilize a state system when setting up

public:
	IHyLoadable(const HyNodePath &nodePath);
	IHyLoadable(const IHyLoadable &copyRef);
	IHyLoadable(IHyLoadable &&donor) noexcept;
	virtual ~IHyLoadable();

	IHyLoadable &operator=(const IHyLoadable &rhs);
	IHyLoadable &operator=(IHyLoadable &&donor);

	const HyNodePath &GetPath() const;
	std::string GetName() const;
	std::string GetPrefix() const;

	virtual uint32 GetState() const;
	virtual bool SetState(uint32 uiStateIndex);
	virtual uint32 GetNumStates();

	const IHyNodeData *AcquireData();

	bool IsLoaded() const;
	HyLoadState GetLoadState() const;
	virtual void Load();
	virtual void Unload();

	virtual bool IsLoadDataValid() { return true; }	// Optional public override for derived classes to indicate that its current state 

protected:
	void _Reinitialize(const HyNodePath &nodePathRef);

	virtual bool IsChildrenLoaded() const { return true; }// Required override for entities to properly determine whether they're loaded

	// Optional overrides for derived classes
	virtual void OnDataAcquired() { }				// Invoked once on the first time this node's data is queried
	virtual void OnLoaded() { }						// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnLoadedUpdate() { }				// Invoked once after OnLoaded(), then once every frame (guaranteed to only be invoked if this instance is loaded)
	virtual void OnUnloaded() { }					// HyAssets invokes this instance's data has been erased

	const IHyNodeData *UncheckedGetData() const;	// Used internally when it's guaranteed that data has already been acquired for this instance

private:
	virtual HyType _LoadableGetType() = 0;
	virtual IHyLoadable *_LoadableGetParentPtr() = 0;
};

#endif /* IHyLoadable_h__ */
