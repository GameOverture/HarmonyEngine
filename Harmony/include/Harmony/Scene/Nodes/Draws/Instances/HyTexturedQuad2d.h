/**************************************************************************
*	HyTexturedQuad2d.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	Harmony License:
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
	bool				m_bIsRaw;	// This is 'true' when using the raw ctor which by-passes the traditional loading and instead takes info directly from the gfx api
	uint32				m_uiAtlasGroupId;
	uint32				m_uiAtlasIndexInGroup;
	uint32				m_uiRawTextureWidth;
	uint32				m_uiRawTextureHeight;

	HyRectangle<float>	m_SrcRect;

public:
	HyTexturedQuad2d(uint32 uiAtlasGrpId, uint32 uiIndexInGroup, HyEntity2d *pParent);
	HyTexturedQuad2d(HyTextureHandle hTextureHandle, uint32 uiTextureWidth, uint32 uiTextureHeight, HyEntity2d *pParent);
	HyTexturedQuad2d(const HyTexturedQuad2d &copyRef);
	virtual ~HyTexturedQuad2d();

	const HyTexturedQuad2d &operator=(const HyTexturedQuad2d &rhs);
	virtual HyTexturedQuad2d *Clone() const override;

	virtual bool IsLoaded() const override;

	void SetTextureSource(int iX, int iY, int iWidth, int iHeight);

	uint32 GetAtlasIndexInGroup();
	uint32 GetWidth();
	uint32 GetHeight();

	uint32 GetEntireTextureWidth();
	uint32 GetEntireTextureHeight();

protected:
	virtual void OnLoaded() override;
	virtual void OnWriteVertexData(char *&pRefDataWritePos) override;
};

#endif /* HyTexturedQuad2d_h__ */
