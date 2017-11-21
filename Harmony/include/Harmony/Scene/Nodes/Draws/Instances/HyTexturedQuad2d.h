/**************************************************************************
*	HyTexturedQuad2d.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyTexturedQuad2d_h__
#define HyTexturedQuad2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"
#include "Utilities/HyMath.h"

class HyTexturedQuad2d : public IHyDrawInst2d
{
protected:
	const bool				m_bIS_RAW;	// This is 'true' when using the raw ctor which by-passes the traditional loading and instead takes info directly from the gfx api
	const uint32			m_uiATLAS_GROUP_ID;
	const uint32			m_uiATLAS_INDEX_IN_GROUP;
	const uint32			m_uiRAW_TEXTURE_WIDTH;
	const uint32			m_uiRAW_TEXTURE_HEIGHT;

	HyRectangle<float>		m_SrcRect;

public:
	HyTexturedQuad2d(uint32 uiAtlasGrpId, uint32 uiIndexInGroup, HyEntity2d *pParent);
	HyTexturedQuad2d(HyTextureHandle hTextureHandle, uint32 uiTextureWidth, uint32 uiTextureHeight, HyEntity2d *pParent);
	virtual ~HyTexturedQuad2d();

	void SetTextureSource(int iX, int iY, int iWidth, int iHeight);

	uint32 GetAtlasIndexInGroup();
	uint32 GetWidth();
	uint32 GetHeight();

	uint32 GetEntireTextureWidth();
	uint32 GetEntireTextureHeight();

protected:
	virtual void CalcBoundingVolume() override;
	virtual void AcquireBoundingVolumeIndex(uint32 &uiStateOut, uint32 &uiSubStateOut) override;

private:
	virtual void DrawUpdate() override;
	virtual void OnUpdateUniforms() override;
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) override;
};

#endif /* HyTexturedQuad2d_h__ */
