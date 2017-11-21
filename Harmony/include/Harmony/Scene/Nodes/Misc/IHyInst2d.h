/**************************************************************************
*	IHyInst2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLeafNode2d_h__
#define IHyLeafNode2d_h__

#include "Scene/Nodes/IHyNode2d.h"

class IHyInst2d : public IHyNode2d
{

public:
	IHyInst2d(HyType eNodeType, HyEntity2d *pParent);
	virtual ~IHyInst2d();

	void SetEnabled(bool bEnabled);
	void SetPauseUpdate(bool bUpdateWhenPaused);

protected:
	virtual void NodeUpdate() = 0;
};

#endif /* IHyLeafNode2d_h__ */
