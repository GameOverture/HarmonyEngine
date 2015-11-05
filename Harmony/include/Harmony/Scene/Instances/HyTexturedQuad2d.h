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

#include "Scene/Instances/IHyInst2d.h"

class HyTexturedQuad2d : public IHyInst2d
{
	uint32		m_uiTextureIndex;

public:
	HyTexturedQuad2d(uint32 uiAtlasGroupIndex);
	virtual ~HyTexturedQuad2d();

	uint32 GetAtlasGroupId() const;

	void SetTextureIndex(uint32 uiTextureIndex);
	uint32 GetTextureIndex();

private:
	virtual void OnDataLoaded();
	virtual void Update();
	virtual void WriteDrawBufferData(char *&pRefDataWritePos);
};

#endif /* __HyTexturedQuad2d_h__ */
