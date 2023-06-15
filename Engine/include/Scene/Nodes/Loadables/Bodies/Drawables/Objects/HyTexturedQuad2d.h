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
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Utilities/HyMath.h"

class HyTexturedQuad2d : public IHyDrawable2d
{
	int32										m_iFullTextureWidth;
	int32										m_iFullTextureHeight;
	HyRectangle<float>							m_UvRect;
	HyTextureHandle								m_hTextureHandle;

public:
	HyTexturedQuad2d(HyEntity2d *pParent = nullptr);
	HyTexturedQuad2d(uint32 uiAtlasFrameChecksum, uint32 uiBankId, HyEntity2d *pParent = nullptr);
	HyTexturedQuad2d(std::string sFilePath, HyTextureInfo useTextureInfo, HyEntity2d *pParent = nullptr);
	HyTexturedQuad2d(const HyTexturedQuad2d &copyRef);
	virtual ~HyTexturedQuad2d();

	const HyTexturedQuad2d &operator=(const HyTexturedQuad2d &rhs);

	void Init(uint32 uiAtlasFrameChecksum, uint32 uiBankId, HyEntity2d *pParent);
	void Init(std::string sFilePath, HyTextureInfo useTextureInfo, HyEntity2d *pParent);
	void Uninit();

	bool IsHotloading() const;

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;

	//void SetUvCoordinates(int iX, int iY, int iWidth, int iHeight);

	uint32 GetWidth() const;
	uint32 GetHeight() const;

	int32 GetEntireTextureWidth() const;
	int32 GetEntireTextureHeight() const;

protected:
	virtual void OnDataAcquired() override;
	virtual void OnLoaded() override;
	virtual void OnUnloaded() override;

	virtual bool OnIsValidToRender() override;

	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;

private: // Hide inherited Init() and Uninit() functionality, since we overload them with different parameters
	using IHyLoadable2d::Init;
	using IHyLoadable2d::Uninit;
};

#endif /* HyTexturedQuad2d_h__ */
