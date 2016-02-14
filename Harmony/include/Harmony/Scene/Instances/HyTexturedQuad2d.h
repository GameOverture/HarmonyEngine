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
#include "Utilities/HyMath.h"

class HyTexturedQuad2d : public IHyInst2d
{
	uint32					m_uiTextureIndex;
	HyRectangle<float>		m_SrcRect;
public:
	HyTexturedQuad2d(uint32 uiAtlasGroupId);
	virtual ~HyTexturedQuad2d();

	uint32 GetAtlasGroupId() const;

	void SetTextureSource(uint32 uiTextureIndex);
	void SetTextureSource(uint32 uiTextureIndex, int iX, int iY, int iWidth, int iHeight);

	uint32 GetTextureIndex();

	uint32 GetTextureWidth();
	uint32 GetTextureHeight();
	uint32 GetNumTextures();

private:
	virtual void OnDataLoaded();
	virtual void Update();
	virtual void WriteDrawBufferData(char *&pRefDataWritePos);
};

#endif /* __HyTexturedQuad2d_h__ */
