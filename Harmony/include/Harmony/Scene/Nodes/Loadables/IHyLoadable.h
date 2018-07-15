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

class IHyLoadable
{
	friend class HyAssets;

protected:
	static HyAssets *				sm_pHyAssets;
	HyLoadState						m_eLoadState;

	const IHyNodeData *				m_pData;
	std::string						m_sName;
	std::string						m_sPrefix;

public:
	IHyLoadable(const char *szPrefix, const char *szName);
	IHyLoadable(const IHyLoadable &copyRef);
	virtual ~IHyLoadable();

	const IHyLoadable &operator=(const IHyLoadable &rhs);

	const std::string &GetName() const;
	const std::string &GetPrefix() const;

	const IHyNodeData *AcquireData();

	virtual bool IsLoaded() const;
	virtual void Load();
	virtual void Unload();

	virtual bool IsLoadDataValid() { return true; }						// Optional public override for derived classes

protected:
	// Optional overrides for derived classes
	virtual void DrawLoadedUpdate() { }									// Invoked once after OnLoaded(), then once every frame (guarenteed to only be invoked if this instance is loaded)
	virtual void OnDataAcquired() { }									// Invoked once on the first time this node's data is queried
	virtual void OnLoaded() { }											// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnUnloaded() { }										// HyAssets invokes this instance's data has been erased

	const IHyNodeData *UncheckedGetData();								// Used internally when it's guaranteed that data has already been acquired for this instance

private:
	virtual HyType _LoadableGetType() = 0;
};

#endif /* IHyLoadable_h__ */
