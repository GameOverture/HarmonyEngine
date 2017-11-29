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

class HyRenderState;
class IHyDrawInst2d;

enum HyStencilBehavior
{
	HYSTENCILBEHAVIOR_Mask = 0,
	HYSTENCILBEHAVIOR_InvertedMask
};

class HyStencil
{
	friend class IHyRenderer;

	static HyStencilHandle			sm_hHandleCount;
	const HyStencilHandle			m_hHANDLE;

	std::vector<IHyDrawInst2d *>	m_InstanceList;
	bool							m_bInstanceListDirty;
	HyRenderState *					m_pRenderStatePtr;

	HyStencilBehavior				m_eBehavior;

public:
	HyStencil();
private: ~HyStencil();
public:
	void Destroy();

	HyStencilHandle GetHandle();

	void AddInstance(IHyDrawInst2d *pInstance);
	bool RemoveInstance(IHyDrawInst2d *pInstance);

	HyStencilBehavior GetBehavior();

	void SetAsMask();
	void SetAsInvertedMask();

	const std::vector<IHyDrawInst2d *> &GetInstanceList();
	HyRenderState *GetRenderStatePtr();

private:
	void SetRenderStatePtr(HyRenderState *pPtr);
};

#endif /* HyStencil_h__ */
