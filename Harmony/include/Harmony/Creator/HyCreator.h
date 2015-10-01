/**************************************************************************
 *	HyCreator.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyCreator_h__
#define __HyCreator_h__

#include "Afx/HyStdAfx.h"

#include "Data/HyFactory.h"
#include "Threading/Threading.h"

#include "Utilities/HyDebugPhys2d.h"
#include "Utilities/HyBox2dRuntime.h"

#include <Box2D/Box2D.h>

#include <vector>
#include <queue>
#include <map>
using std::vector;
using std::queue;
using std::map;

// Forward declarations
class IObjInst2d;

class HySound;
class HySpine2d;
class HyMesh3d;
class HyText2d;
class HyPrimitive2d;

class IHyData;
class HySfxData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyMesh3dData;

class HyGfxComms;
class HyPhysics2d;
class HyAnimVec2;
class HyAnimVec3;

class HyViewport;
class HyEntity2d;
class HyAnimFloat;

//////////////////////////////////////////////////////////////////////////
class HyCreator
{
	friend class HyEngine;

	static HyCoordinateType								sm_eDefaultCoordType;
	static float										sm_fPixelsPerMeter;

	// Thread control structure to help sync loading of factory data
	struct LoadThreadCtrl
	{
		WaitEvent		m_WaitEvent_HasNewData;

		queue<IHyData *> *m_pLoadQueue_Shared;
		queue<IHyData *> *m_pLoadQueue_Retrieval;

		BasicSection	m_csSharedQueue;
		BasicSection	m_csRetrievalQueue;

        LoadThreadCtrl() : m_WaitEvent_HasNewData(L"Thread Idler", true)
		{}
	};
	LoadThreadCtrl										m_LoadingCtrl;

	b2World												m_b2World;
	int32												m_iPhysVelocityIterations;
	int32												m_iPhysPositionIterations;
	HyDebugPhys2d										m_DrawPhys2d;
	HyBox2dRuntime										m_Phys2dContactListener;

	HyGfxComms &										m_GfxCommsRef;
	HyViewport &										m_ViewportRef;

	HyFactory<HySfxData>								m_Sfx;
	HyFactory<HySprite2dData>							m_Sprite2d;
	HyFactory<HySpine2dData>							m_Spine2d;
	HyFactory<HyMesh3dData>								m_Mesh3d;
	HyFactory<HyText2dData>								m_Txt2d;
	HyFactory<HyTexturedQuad2dData>						m_Quad2d;

	// Pointers to all instances
	bool												m_bInst2dOrderingDirty;
	vector<IObjInst2d *>								m_vQueuedInst2d;
	vector<IObjInst2d *>								m_vLoadedInst2d;

	vector<IObjInst2d *>								m_vInst3d;

	vector<HyAnimFloat *>								m_vActiveAnimFloats;

	// Used when writing the graphics draw buffer
	char *												m_pCurWritePos;

	// Queues responsible for passing and retrieving factory data between the loading thread
	queue<IHyData *>										m_LoadQueue_Prepare;
	queue<IHyData *>										m_LoadQueue_Shared;
	queue<IHyData *>										m_LoadQueue_Retrieval;

	queue<IHyData *> *									m_pGfxQueue_Retrieval;

	// Loading thread info pointer
	ThreadInfoPtr										m_pLoadingThread;

public:
	HyCreator(HyGfxComms &gfxCommsRef, HyViewport &gameViewport, HyCoordinateType eDefaultCoordType, float fPixelsPerMeter);
	~HyCreator(void);

	static float PixelsPerMeter()					{ return sm_fPixelsPerMeter; }
	static HyCoordinateType DefaultCoordinateType()	{ return sm_eDefaultCoordType; }

	void DebugDrawPhysics2d(bool bDraw)				{ m_DrawPhys2d.SetDrawEnabled(bDraw); }

	void SetInstOrderingDirty()						{ m_bInst2dOrderingDirty = true; }

	void InsertActiveAnimFloat(HyAnimFloat *pAnimFloat);

	void LoadInst2d(IObjInst2d *pInst);
	void RemoveInst(IObjInst2d *pInst);

private:
	void PreUpdate();
	void PostUpdate();

	void UpdateLoading();
	void OnDataLoaded(IHyData *pData);
	void DiscardData(IHyData *pData);
	void DeleteData(IHyData *pData);
	
	void WriteDrawBuffers();
	
	static void LoadingThread(void *pParam);
	static bool Inst2dSortPredicate(const IObjInst2d *pInst1, const IObjInst2d *pInst2);
};

#endif /* __HyCreator_h__ */

