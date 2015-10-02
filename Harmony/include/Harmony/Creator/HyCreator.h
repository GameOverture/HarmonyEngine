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

#include "Utilities/HyDebugPhys2d.h"
#include "Utilities/HyBox2dRuntime.h"

#include <Box2D/Box2D.h>

#include <vector>
using std::vector;

// Forward declarations
class IHyInst2d;

class HySound;
class HySpine2d;
class HyMesh3d;
class HyText2d;
class HyPrimitive2d;

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

	b2World												m_b2World;
	int32												m_iPhysVelocityIterations;
	int32												m_iPhysPositionIterations;
	HyDebugPhys2d										m_DrawPhys2d;
	HyBox2dRuntime										m_Phys2dContactListener;

	HyGfxComms &										m_GfxCommsRef;
	HyViewport &										m_ViewportRef;

	// Pointers to all instances
	bool												m_bInst2dOrderingDirty;
	
	vector<IHyInst2d *>								m_vLoadedInst2d;

	vector<IHyInst2d *>								m_vInst3d;

	vector<HyAnimFloat *>								m_vActiveAnimFloats;

	// Used when writing the graphics draw buffer
	char *												m_pCurWritePos;

public:
	HyCreator(HyGfxComms &gfxCommsRef, HyViewport &gameViewport, HyCoordinateType eDefaultCoordType, float fPixelsPerMeter);
	~HyCreator(void);

	static float PixelsPerMeter()					{ return sm_fPixelsPerMeter; }
	static HyCoordinateType DefaultCoordinateType()	{ return sm_eDefaultCoordType; }

	void DebugDrawPhysics2d(bool bDraw)				{ m_DrawPhys2d.SetDrawEnabled(bDraw); }

	void SetInstOrderingDirty()						{ m_bInst2dOrderingDirty = true; }

	void InsertActiveAnimFloat(HyAnimFloat *pAnimFloat);

private:
	void PreUpdate();
	void PostUpdate();
	
	void WriteDrawBuffers();
	
	static bool Inst2dSortPredicate(const IHyInst2d *pInst1, const IHyInst2d *pInst2);
};

#endif /* __HyCreator_h__ */

