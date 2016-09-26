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
protected:
	const bool				m_bIS_RAW;	// This is 'true' when using the raw ctor which by-passes the traditional loading and instead takes info directly from the gfx api
	const uint32			m_uiRAW_TEXTURE_WIDTH;
	const uint32			m_uiRAW_TEXTURE_HEIGHT;

	uint32					m_uiTextureIndex;
	HyRectangle<float>		m_SrcRect;

public:
	HyTexturedQuad2d(uint32 uiAtlasGroupId);
	HyTexturedQuad2d(uint32 uiGfxApiHandle, uint32 uiTextureWidth, uint32 uiTextureHeight);	// Note: The 'uiGfxApiHandle' must be a handle to a texture array
	virtual ~HyTexturedQuad2d();

	uint32 GetAtlasGroupId() const;
	uint32 GetGraphicsApiHandle() const;

	void SetTextureSource(uint32 uiTextureIndex);
	void SetTextureSource(uint32 uiTextureIndex, int iX, int iY, int iWidth, int iHeight);

	uint32 GetTextureIndex();
	uint32 GetWidth();
	uint32 GetHeight();

	uint32 GetEntireTextureWidth();
	uint32 GetEntireTextureHeight();
	uint32 GetNumTextures();

private:
	virtual void OnDataLoaded();

	virtual void OnInstUpdate();
	
	virtual void OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef);
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

#endif /* __HyTexturedQuad2d_h__ */
