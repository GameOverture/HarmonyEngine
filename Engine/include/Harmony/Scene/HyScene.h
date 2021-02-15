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
#include "Audio/Harness/IHyAudioCore.h"

// Forward declarations
class IHyNode;
class IHyNode2d;
class IHyNode3d;
class HyEntity2d;
class HyEntity3d;
class IHyDrawable2d;
class IHyDrawable3d;
class HyWindow;
class IHyRenderer;

//////////////////////////////////////////////////////////////////////////
class HyScene
{
	IHyAudioCore &										m_AudioCoreRef;
	std::vector<HyWindow *> &							m_WindowListRef;

	// TODO: Make tightly packed (memory contiguous) node arrays that holds the "Hot" data needed to be updated and drawn
	static std::vector<IHyNode *>						sm_NodeList_All;
	static std::vector<IHyNode *>						sm_NodeList_PauseUpdate;		// List of nodes who will update when the game is paused
	bool												m_bPauseGame;

	static std::vector<HyPhysicsGrid2d *>				sm_PhysicsGridList;

	// List of nodes who can be drawn, and their graphics assets are fully loaded
	static bool											sm_bInst2dOrderingDirty;
	std::vector<IHyDrawable2d *>						m_NodeList_LoadedDrawable2d;
	std::vector<IHyDrawable3d *>						m_NodeList_LoadedDrawable3d;

public:
	HyScene(IHyAudioCore &audioCoreRef, std::vector<HyWindow *> &WindowListRef);
	~HyScene(void);

	static void SetInstOrderingDirty();
	
	static void AddNode(IHyNode *pNode);
	static void RemoveNode(IHyNode *pNode);

	static void AddNode_PauseUpdate(IHyNode *pNode);
	static void RemoveNode_PauseUpdate(IHyNode *pNode);

	static void AddPhysicsGrid(HyPhysicsGrid2d *pPhysGrid);
	static void RemovePhysicsGrid(HyPhysicsGrid2d *pPhysGrid);

	void AddNode_Loaded(IHyDrawable2d *pDrawable);
	void AddNode_Loaded(IHyDrawable3d *pDrawable);
	void RemoveNode_Loaded(const IHyDrawable2d *pDrawable);
	void RemoveNode_Loaded(const IHyDrawable3d *pDrawable);
	void CopyAllLoadedNodes(std::vector<IHyDrawable2d *> &nodeListOut);

	void AppendAudioCue(IHyNode *pNode, IHyAudioCore::CueType eCueType);

	void SetPause(bool bPause);

	void UpdateNodes();
	void UpdatePhysics();
	void PrepareRender(IHyRenderer &rendererRef);

	bool CalculateCameraMask(/*const*/ IHyDrawable2d &instanceRef, uint32 &uiCameraMaskOut) const;
	
	static bool Node2dSortPredicate(const IHyDrawable2d *pInst1, const IHyDrawable2d *pInst2);
};

#endif /* HyScene_h__ */
