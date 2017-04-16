/**************************************************************************
*	HyScissor.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyScissor_h__
#define HyScissor_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Utilities/HyMath.h"

class HyScissor
{
	IHyNode2d &						m_OwnerRef;
	
	HyScreenRect<int32>				m_LocalScissorRect;
	HyScreenRect<int32>				m_WorldScissorRect;

public:
	HyScissor(IHyNode2d &ownerRef);
	~HyScissor();

	HyScissor &operator =(HyScissor &rightSideRef);

	bool IsEnabled();
	const HyScreenRect<int32> &GetLocal();
	void Set(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight);
	void Clear();

	const HyScreenRect<int32> &GetWorld();
};

#endif /* HyScissor_h__ */
