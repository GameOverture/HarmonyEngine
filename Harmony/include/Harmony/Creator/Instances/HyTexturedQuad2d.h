/**************************************************************************
*	HyTexturedQuad2d.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyTexturedQuad2d_h__
#define __HyTexturedQuad2d_h__

#include "Afx/HyStdAfx.h"
#include "Creator/Instances/IObjInst2d.h"
#include "Creator/Data/HyTexturedQuad2dData.h"

#include "FileIO/Atlas/HyTexture.h"

class HyTexturedQuad2d : public IObjInst2d
{
public:
	HyTexturedQuad2d(uint32 uiTextureIndex);
	virtual ~HyTexturedQuad2d();

private:
	virtual void OnDataLoaded();
	virtual void Update();
	virtual void WriteDrawBufferData(char *&pRefDataWritePos);
};

#endif /* __HyTexturedQuad2d_h__ */
