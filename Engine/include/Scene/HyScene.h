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
#include "Audio/HyAudioCore.h"

// Forward declarations
class IHyNode;
class IHyNode2d;
class IHyNode3d;
class HyEntity2d;
class HyEntity3d;
class IHyLoadable;
class IHyDrawable2d;
class IHyDrawable3d;
class HyWindow;
class IHyRenderer;

//////////////////////////////////////////////////////////////////////////
class HyScene
{
	HyAudioCore &										m_AudioCoreRef;
	std::vector<HyWindow *> &							m_WindowListRef;

	// TODO: Make tightly packed (memory contiguous) node arrays that holds the "Hot" data needed to be updated and drawn
	static std::vector<IHyNode *>						sm_NodeList_All;
	static std::vector<IHyNode *>						sm_NodeList_PauseUpdate;		// List of nodes who will update when the game is paused
	bool												m_bPauseGame;

	// List of nodes who can be drawn, and their graphics assets are fully loaded
	static bool											sm_bInst2dOrderingDirty;
	std::vector<IHyDrawable2d *>						m_NodeList_LoadedDrawable2d;
	std::vector<IHyDrawable3d *>						m_NodeList_LoadedDrawable3d;

public:
	HyScene(HyAudioCore &audioCoreRef, std::vector<HyWindow *> &WindowListRef);
	~HyScene(void);

	static void SetInstOrderingDirty();
	
	static void AddNode(IHyNode *pNode);
	static void RemoveNode(IHyNode *pNode);

	static void AddNode_PauseUpdate(IHyNode *pNode);
	static void RemoveNode_PauseUpdate(IHyNode *pNode);

	void AddNode_Loaded(IHyDrawable2d *pDrawable);
	void AddNode_Loaded(IHyDrawable3d *pDrawable);
	void RemoveNode_Loaded(const IHyDrawable2d *pDrawable);
	void RemoveNode_Loaded(const IHyDrawable3d *pDrawable);
	void CopyAllLoadedNodes(std::vector<IHyLoadable *> &nodeListOut);

	void ProcessAudioCue(IHyNode *pNode, HySoundCue eCueType);

	void SetPause(bool bPause);

	void UpdateNodes();
	void PrepareRender(IHyRenderer &rendererRef, float fExtrapolatePercent);

	bool CalculateCameraMask(/*const*/ IHyDrawable2d &instanceRef, uint32 &uiCameraMaskOut) const;
	
	static bool Node2dSortPredicate(IHyDrawable2d *pInst1, IHyDrawable2d *pInst2);
};

#endif /* HyScene_h__ */
