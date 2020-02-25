/**************************************************************************
*	HyStencil.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyStencil_h__
#define HyStencil_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/Components/HyRenderBuffer.h"

class IHyInstance2d;

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

	// TODO: Support 3d drawables too
	std::vector<IHyInstance2d *>	m_MaskInstanceList;
	bool							m_bMaskIsReady;
	HyRenderBuffer::State *			m_pRenderStatePtr;

	HyStencilBehavior				m_eBehavior;

public:
	HyStencil();
	~HyStencil();

	HyStencilHandle GetHandle() const;

	// It's the user's responsibility to ensure added instances continue to be valid
	void AddMask(IHyInstance2d &nodeRef);
	bool RemoveMask(IHyInstance2d &nodeRef);

	bool IsMaskReady();

	HyStencilBehavior GetBehavior() const;

	void SetAsMask();
	void SetAsInvertedMask();

	const std::vector<IHyInstance2d *> &GetInstanceList() const;
	HyRenderBuffer::State *GetRenderStatePtr() const;

private:
	bool ConfirmMaskReady();
	void SetRenderStatePtr(HyRenderBuffer::State *pPtr);
};

#endif /* HyStencil_h__ */
