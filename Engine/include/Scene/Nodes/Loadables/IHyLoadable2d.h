/**************************************************************************
*	IHyLoadable2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLoadable2d_h__
#define IHyLoadable2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Scene/AnimFloats/HyAnimVec3.h"

class IHyLoadable2d : public IHyNode2d, public IHyLoadable
{
public:
	IHyLoadable2d(HyType eNodeType, const HyNodePath &nodePath, HyEntity2d *pParent);
	IHyLoadable2d(const IHyLoadable2d &copyRef);
	IHyLoadable2d(IHyLoadable2d &&donor) noexcept;
	virtual ~IHyLoadable2d();

	IHyLoadable2d &operator=(const IHyLoadable2d &rhs);
	IHyLoadable2d &operator=(IHyLoadable2d &&donor);

	virtual void Init(const HyNodePath &nodePath, HyEntity2d *pParent);
	virtual void Init(const char *szPrefix, const char *szName, HyEntity2d *pParent);
	virtual void Uninit();

protected:
	virtual void Update() override;

private:
	virtual HyType _LoadableGetType() override final;
	virtual IHyLoadable *_LoadableGetParentPtr() override final;
};

#endif /* IHyLoadable2d_h__ */
