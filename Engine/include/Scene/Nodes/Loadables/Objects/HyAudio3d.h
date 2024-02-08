/**************************************************************************
 *	HyAudio3d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudio3d_h__
#define HyAudio3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Objects/IHyAudio.h"
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"

class HyAudio3d : public IHyAudio<IHyLoadable3d, HyEntity3d>
{
public:
	HyAudio3d(HyEntity3d *pParent = nullptr);
	HyAudio3d(const HyNodePath &nodePath, HyEntity3d *pParent = nullptr);
	HyAudio3d(const char *szPrefix, const char *szName, HyEntity3d *pParent = nullptr);
	// TODO: copy ctor and move ctor
	virtual ~HyAudio3d(void);
	// TODO: assignment operator and move operator

private:
	// Hide visibilty modifiers because we're sound
	using IHyNode::IsVisible;
	using IHyNode::SetVisible;
};

#endif /* HyAudio3d_h__ */
