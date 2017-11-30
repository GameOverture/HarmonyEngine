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

struct HyPortal2dInit
{
	glm::vec2 ptGateA1;
	glm::vec2 ptGateA2;
	glm::vec2 ptGateA_EntranceHalfSpace;	// A point that indicates which half-space of gate A's segment is the entrance into the portal

	glm::vec2 ptGateB1;
	glm::vec2 ptGateB2;
	glm::vec2 ptGateB_EntranceHalfSpace;	// A point that indicates which half-space of gate B's segment is the entrance into the portal
	
	float fGateDepthAmt;					// How "deep" the portal is (in pixels), which is a collision volume that determines what is inside the portal
	float fStencilCullExtents;				// A value that should be larger than either width or height of the framebuffer size of the window
	
	HyPortal2dInit() :	fGateDepthAmt(100.0f),
						fStencilCullExtents(5000.0f)
	{ }

	void SetGateA(glm::vec2 pt1, glm::vec2 pt2, glm::vec2 ptEntrance)
	{
		ptGateA1 = pt1;
		ptGateA2 = pt2;
		ptGateA_EntranceHalfSpace = ptEntrance;
	}

	void SetGateB(glm::vec2 pt1, glm::vec2 pt2, glm::vec2 ptEntrance)
	{
		ptGateB1 = pt1;
		ptGateB2 = pt2;
		ptGateB_EntranceHalfSpace = ptEntrance;
	}
};

class HyPortal2d
{
	friend class IHyRenderer;
	friend class HyRenderState;
	friend class IHyDrawInst2d;

	static HyPortal2dHandle						sm_hHandleCount;
	const HyPortal2dHandle						m_hHANDLE;

	HyPortalGate2d								m_GateA;
	HyPortalGate2d								m_GateB;

	std::vector<IHyDrawInst2d *>				m_DrawInstList;
	std::vector<IHyDrawInst2d *>				m_CloneInstList;

public:
	HyPortal2d(const HyPortal2dInit &initRef);
private: ~HyPortal2d();
public:
	void Destroy();

	HyPortal2dHandle GetHandle() const;

	void EnableDebugDraw(bool bEnable);

private:
	void AddInstance(IHyDrawInst2d *pInstance);
	bool RemoveInstance(IHyDrawInst2d *pInstance);

	void PrepareClones();
};

#endif /* HyPortal2d_h__ */
