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

class IHyDrawable2d;
class IHyBody2d;

enum HyStencilBehavior
{
	HYSTENCILBEHAVIOR_Scissor = 0,
	HYSTENCILBEHAVIOR_Mask,					// Only pixels within the stencil will be drawn
	HYSTENCILBEHAVIOR_InvertedMask,			// Only pixels outside the stencil will be drawn
	
	// NOTE: Any new behaviors need to be properly implemented in HyOpenGL::DrawRenderState_2d
};

class HyStencil
{
	friend class IHyRenderer;
	friend class IHyBody2d;

	static HyStencilHandle			sm_hHandleCount;
	const HyStencilHandle			m_hHANDLE;

	// TODO: Support 3d drawables too
	std::vector<IHyDrawable2d *>	m_MaskInstanceList;
	bool							m_bMaskIsReady;
	HyRenderBuffer::State *			m_pRenderStatePtr;

	HyStencilBehavior				m_eBehavior;
	IHyBody2d *						m_pScissorOwner;

public:
	HyStencil();
	~HyStencil();

	HyStencilHandle GetHandle() const;

	// It's the user's responsibility to ensure added instances continue to be valid
	void AddMask(IHyDrawable2d &nodeRef);
	bool RemoveMask(IHyDrawable2d &nodeRef);

	bool IsMaskReady();

	HyStencilBehavior GetBehavior() const;

	void SetAsMask();
	void SetAsInvertedMask();

	const std::vector<IHyDrawable2d *> &GetInstanceList() const;
	HyRenderBuffer::State *GetRenderStatePtr() const;

private:
	void SetAsScissor(const HyRect &scissorRect, IHyBody2d *pScissorOwner);

	bool ConfirmMaskReady();
	void PrepRender(HyRenderBuffer::State *pPtr, float fExtrapolatePercent);
};

#endif /* HyStencil_h__ */
