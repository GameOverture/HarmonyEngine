/**************************************************************************
 *	IHyEntity2d.h
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

class IHyEntity2d : public IHyInst2d
{
public:
	IHyEntity2d();
	IHyEntity2d(const char *szPrefix, const char *szName);
	virtual ~IHyEntity2d(void);

private:
	virtual void OnUpdate() = 0;
};

#endif /* __HyEntity2d_h__ */
