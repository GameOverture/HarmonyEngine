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

public:
	HyEntity2d();
	HyEntity2d(const char *szPrefix, const char *szName);
	virtual ~HyEntity2d(void);

	virtual void SetEnabled(bool bEnabled);
	
	virtual bool IsLoaded();

	virtual void SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom);

	int32 GetDisplayOrderMax();
	virtual void SetDisplayOrder(int32 iOrderValue);

	virtual void Load();
	virtual void Unload();

private:
	// Unused virtual overrides
	virtual void OnUpdate() { }
	virtual void OnInstUpdate() { }
	virtual void OnDataLoaded() { }
	virtual void OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef) { }
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) { }
};

#endif /* __HyEntity2d_h__ */
