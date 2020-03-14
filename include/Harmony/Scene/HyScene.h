/**************************************************************************
 *	HyScene.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyScene_h__
#define HyScene_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyPhysicsGrid2d.h"
#include "Scene/Physics/HyBox2dRuntime.h"
#include "Scene/Physics/HyPhysicsDebug2d.h"

// Forward declarations
class IHyNode;
class IHyNode2d;
class IHyNode3d;
class HyEntity2d;
class HyEntity3d;
class IHyInstance2d;
class IHyInstance3d;
class HyWindow;
class IHyRenderer;

//////////////////////////////////////////////////////////////////////////
class HyScene
{
	static bool											sm_bInst2dOrderingDirty;

	std::vector<HyWindow *> &							m_WindowListRef;

	// TODO: Make tightly packed (memory contiguous) node arrays that holds the "Hot" data needed to be updated and drawn
	static std::vector<IHyNode *>						sm_NodeList_All;
	static std::vector<IHyNode *>						sm_NodeList_PauseUpdate;		// List of nodes who will update when the game is paused
	bool												m_bPauseGame;

	static std::vector<HyPhysicsGrid2d *>					sm_PhysicsGridList;

	// List of nodes who can be drawn, and their graphics assets are fully loaded
	std::vector<IHyInstance2d *>						m_NodeList_LoadedDrawable2d;
	std::vector<IHyInstance3d *>						m_NodeList_LoadedDrawable3d;

public:
	HyScene(std::vector<HyWindow *> &WindowListRef);
	~HyScene(void);

	static void SetInstOrderingDirty();
	
	static void AddNode(IHyNode *pNode);
	static void RemoveNode(IHyNode *pNode);

	static void AddNode_PauseUpdate(IHyNode *pNode);
	static void RemoveNode_PauseUpdate(IHyNode *pNode);

	static void AddPhysicsGrid(HyPhysicsGrid2d *pPhysGrid);
	static void RemovePhysicsGrid(HyPhysicsGrid2d *pPhysGrid);

	void AddNode_Loaded(IHyInstance2d *pDrawable);
	void AddNode_Loaded(IHyInstance3d *pDrawable);
	void RemoveNode_Loaded(const IHyInstance2d *pDrawable);
	void RemoveNode_Loaded(const IHyInstance3d *pDrawable);

	void CopyAllLoadedNodes(std::vector<IHyInstance2d *> &nodeListOut);

	void SetPause(bool bPause);

	void UpdatePhysics();
	void UpdateNodes();
	void PrepareRender(IHyRenderer &rendererRef);

	bool CalculateCameraMask(/*const*/ IHyInstance2d &instanceRef, uint32 &uiCameraMaskOut) const;
	
	static bool Node2dSortPredicate(const IHyInstance2d *pInst1, const IHyInstance2d *pInst2);
};

#endif /* HyScene_h__ */
