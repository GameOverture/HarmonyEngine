/**************************************************************************
 *	HyEntity2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEntity2d_h__
#define __HyEntity2d_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Instances/HyPrimitive2d.h"
#include "Scene/Instances/HyText2d.h"
#include "Scene/Instances/HySprite2d.h"
#include "Scene/Instances/HySpine2d.h"

#include "Utilities/HyMath.h"

class HyEntity2d : public IHyInst2d
{
	int32					m_iDisplayOrderMax;

	float					m_fPrevAlphaValue;

public:
	HyEntity2d();
	HyEntity2d(const char *szPrefix, const char *szName);
	virtual ~HyEntity2d(void);

	virtual void SetEnabled(bool bEnabled);
	
	virtual bool IsLoaded() const;

	virtual void SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom);

	int32 GetDisplayOrderMax();
	virtual void SetDisplayOrder(int32 iOrderValue);

	virtual void Load();
	virtual void Unload();

private:
	virtual void OnInstUpdate();
	virtual void OnEntityUpdate() = 0;

	// Unused overrides
	virtual void OnDataLoaded() { }
	virtual void OnUpdateUniforms() { }
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) { }
};

#endif /* __HyEntity2d_h__ */
