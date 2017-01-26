/**************************************************************************
 *	HyScene.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyScene_h__
#define __HyScene_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/HyDebugPhys2d.h"
#include "Utilities/HyBox2dRuntime.h"

#include <vector>

// Forward declarations
class IHyTransformNode;
class IHyInst2d;

class HySound;
class HySpine2d;
class HyMesh3d;
class HyText2d;
class HyPrimitive2d;

class HyGfxComms;
class HyPhysics2d;
class HyTweenVec2;
class HyTweenVec3;

class HyWindow;
class HyEntity2d;
class HyTweenFloat;

//////////////////////////////////////////////////////////////////////////
class HyScene
{
	friend class HyEngine;

	static bool											sm_bInst2dOrderingDirty;

	b2World												m_b2World;
	int32												m_iPhysVelocityIterations;
	int32												m_iPhysPositionIterations;
	HyDebugPhys2d										m_DrawPhys2d;
	HyBox2dRuntime										m_Phys2dContactListener;

	HyGfxComms &										m_GfxCommsRef;
	std::vector<HyWindow *> &							m_WindowListRef;

	static std::vector<IHyTransformNode *>				sm_MasterList;

	std::vector<IHyInst2d *>							m_LoadedInst2dList;
	std::vector<IHyInst2d *>							m_LoadedInst3dList;

	// Used when writing the graphics draw buffer
	char *												m_pCurWritePos;

public:
	HyScene(HyGfxComms &gfxCommsRef, std::vector<HyWindow *> &WindowListRef);
	~HyScene(void);

	static void SetInstOrderingDirty()						{ sm_bInst2dOrderingDirty = true; }
	
	static void AddTransformNode(IHyTransformNode *pNode);
	static void RemoveTransformNode(IHyTransformNode *pNode);

	void AddInstance(IHyInst2d *pInst);
	void RemoveInst(IHyInst2d *pInst);

	void CopyAllInsts(std::vector<IHyInst2d *> &vInstsToCopy);

	void AddEntity(HyEntity2d *pEnt);
	void RemoveEntity(HyEntity2d *pEnt);

	void DebugDrawPhysics2d(bool bDraw)					{ m_DrawPhys2d.SetDrawEnabled(bDraw); }

private:
	void PreUpdate();
	void PostUpdate();
	
	void WriteDrawBuffer();
	
	static bool Inst2dSortPredicate(const IHyInst2d *pInst1, const IHyInst2d *pInst2);
};

#endif /* __HyScene_h__ */

