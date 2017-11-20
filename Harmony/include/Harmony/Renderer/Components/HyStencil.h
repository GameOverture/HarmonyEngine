/**************************************************************************
*	HyStencil.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyStencil_h__
#define HyStencil_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Leafs/Draws/HyPrimitive2d.h"

class HyRenderState;

class HyStencil
{
	friend class IHyRenderer;

	static HyStencilHandle			sm_hHandleCount;

	const HyStencilHandle			m_hHANDLE;
	std::vector<IHyLeafDraw2d *>	m_InstanceList;

	char *							m_pRenderStateBuffer;
	bool							m_bInstanceListDirty;

	HyRenderState *					m_pRenderStatePtr;

public:
	HyStencil();
	~HyStencil();

	HyStencilHandle GetHandle();

	void AddInstance(IHyLeafDraw2d *pInstance);
	bool RemoveInstance(IHyLeafDraw2d *pInstance);

	void SetAsCullMask();
	void SetAsInvertedCullMask();

private:
	const std::vector<IHyLeafDraw2d *> &GetInstanceList();
	void SetRenderStatePtr(HyRenderState *pPtr);
};

#endif /* HyStencil_h__ */
