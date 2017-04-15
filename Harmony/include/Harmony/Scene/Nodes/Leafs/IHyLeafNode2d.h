/**************************************************************************
*	IHyLeafNode2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyLeafNode2d_h__
#define __IHyLeafNode2d_h__

#include "Scene/Nodes/IHyNode2d.h"

class IHyLeafNode2d : public IHyNode2d
{

public:
	IHyLeafNode2d(HyType eNodeType, HyEntity2d *pParent);
	virtual ~IHyLeafNode2d();

	void SetEnabled(bool bEnabled);
	void SetPauseUpdate(bool bUpdateWhenPaused);
};

#endif /* __IHyLeafNode2d_h__ */
