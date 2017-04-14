/**************************************************************************
 *	HyEntity2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEntity2d_h__
#define __HyEntity2d_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Nodes/Draws/IHyDraw2d.h"

class HyEntity2d : public IHyDraw2d
{
public:
	HyEntity2d(IHyNode2d *pParent = nullptr);
	HyEntity2d(const char *szPrefix, const char *szName, IHyNode2d *pParent = nullptr);
	virtual ~HyEntity2d(void);

	// Optional user overrides below
	virtual void OnUpdate() { };
	virtual void OnCalcBoundingVolume() { }				// Should calculate the local bounding volume in 'm_BoundingVolume'
	virtual void OnMouseEnter(void *pUserParam) { }
	virtual void OnMouseLeave(void *pUserParam) { }
	virtual void OnMouseDown(void *pUserParam) { }
	virtual void OnMouseUp(void *pUserParam) { }
	virtual void OnMouseClicked(void *pUserParam) { }
};

#endif /* __HyEntity2d_h__ */
