/**************************************************************************
*	HyPortal2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPortal2d_h__
#define HyPortal2d_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/Components/HyPortalGate2d.h"

class HyPortal2d
{
	friend class IHyRenderer;
	friend class HyRenderState;
	friend class IHyDrawInst2d;

	static HyPortal2dHandle						sm_hHandleCount;
	const HyPortal2dHandle						m_hHANDLE;

	HyPortalGate2d								m_Gate1;
	HyPortalGate2d								m_Gate2;

	std::vector<IHyDrawInst2d *>				m_DrawInstList;
	std::vector<IHyDrawInst2d *>				m_CloneInstList;

public:
	HyPortal2d(const HyPortalGate2d &gate1Ref, const HyPortalGate2d &gate2Ref);
private: ~HyPortal2d();
public:
	void Destroy();

	HyPortal2dHandle GetHandle() const;

private:
	void AddInstance(IHyDrawInst2d *pInstance);
	bool RemoveInstance(IHyDrawInst2d *pInstance);

	void PrepareClones();
};

#endif /* HyPortal2d_h__ */
