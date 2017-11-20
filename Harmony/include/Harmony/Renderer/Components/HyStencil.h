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

class HyStencil
{
	static HyStencilHandle			sm_hHandleCount;

	const HyStencilHandle			m_hHANDLE;
	std::vector<IHyLeafDraw2d *>	m_InstanceList;

	char *							m_pRenderStateBuffer;
	bool							m_bInstanceListDirty;

public:
	HyStencil();
	~HyStencil();

	HyStencilHandle GetHandle();

	void AddInstance(IHyLeafDraw2d *pInstance);
	bool RemoveInstance(IHyLeafDraw2d *pInstance);

	void SetAsCullMask();
	void SetAsInvertedCullMask();
};

#endif /* HyStencil_h__ */
