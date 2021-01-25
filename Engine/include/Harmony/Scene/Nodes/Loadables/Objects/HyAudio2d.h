/**************************************************************************
 *	HyAudio2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudio2d_h__
#define HyAudio2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Objects/IHyAudio.h"
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"

class HyAudio2d : public IHyAudio<IHyLoadable2d, HyEntity2d>
{
public:
	HyAudio2d();
	// TODO: copy ctor and move ctor
	virtual ~HyAudio2d(void);
	// TODO: assignment operator and move operator

private:
	// Hide any transform functionality inherited from IHyNode2d
	using IHyNode2d::GetLocalTransform;
	using IHyNode2d::GetWorldTransform;
	using IHyNode2d::GetWorldAABB;
	using IHyNode2d::pos;
	using IHyNode2d::rot;
	using IHyNode2d::rot_pivot;
	using IHyNode2d::scale;
	using IHyNode2d::scale_pivot;

	using IHyNode::IsVisible;
	using IHyNode::SetVisible;
};

#endif /* HyAudio2d_h__ */
