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

#include "Scene/Nodes/Draws/IHyDraw2d.h"
#include "Utilities/HyMath.h"

class HyTexturedQuad2d : public IHyDraw2d
{
protected:
	const bool				m_bIS_RAW;	// This is 'true' when using the raw ctor which by-passes the traditional loading and instead takes info directly from the gfx api
	const uint32			m_uiATLAS_INDEX;
	const uint32			m_uiRAW_TEXTURE_WIDTH;
	const uint32			m_uiRAW_TEXTURE_HEIGHT;

	HyRectangle<float>		m_SrcRect;

public:
	HyTexturedQuad2d(uint32 uiAtlasIndex, IHyNode2d *pParent = nullptr);
	HyTexturedQuad2d(uint32 uiGfxApiHandle, uint32 uiTextureWidth, uint32 uiTextureHeight, IHyNode2d *pParent = nullptr);
	virtual ~HyTexturedQuad2d();

	uint32 GetGraphicsApiHandle() const;

	void SetTextureSource(int iX, int iY, int iWidth, int iHeight);

	uint32 GetAtlasIndex();
	uint32 GetWidth();
	uint32 GetHeight();

	uint32 GetEntireTextureWidth();
	uint32 GetEntireTextureHeight();

private:
	virtual void OnUpdate() override;
	virtual void OnUpdateUniforms() override;
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) override;
};

#endif /* __HyTexturedQuad2d_h__ */
