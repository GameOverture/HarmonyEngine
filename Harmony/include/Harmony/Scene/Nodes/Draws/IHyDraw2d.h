/**************************************************************************
*	IHyDraw2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNodeDraw2d_h__
#define IHyNodeDraw2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Scene/Tweens/HyTweenVec3.h"

class IHyDraw2d : public IHyNode2d
{
	friend class HyAssets;

protected:
	static HyAssets *				sm_pHyAssets;

	HyLoadState						m_eLoadState;

	const IHyNodeData *				m_pData;
	std::string						m_sName;
	std::string						m_sPrefix;

	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTopColor;
	glm::vec3						m_CachedBotColor;

	enum ScissorTag
	{
		SCISSORTAG_Disabled = 0,
		SCISSORTAG_Enabled,
	};
	struct ScissorRect
	{
		HyScreenRect<int32>			m_LocalScissorRect;
		HyScreenRect<int32>			m_WorldScissorRect;
	};
	ScissorRect *					m_pScissor;
	HyStencilHandle					m_hStencil;

	int32							m_iCoordinateSystem;	// -1 (or any negative value) means using world/camera coordinates. Otherwise it represents the Window index
	int32							m_iDisplayOrder;		// Higher values are displayed front-most

public:
	HyTweenVec3						topColor;
	HyTweenVec3						botColor;
	HyTweenFloat					alpha;

public:
	IHyDraw2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent);
	IHyDraw2d(const IHyDraw2d &copyRef);
	virtual ~IHyDraw2d();

	const IHyDraw2d &operator=(const IHyDraw2d &rhs);

	const std::string &GetName() const;
	const std::string &GetPrefix() const;

	const IHyNodeData *AcquireData();

	virtual bool IsLoaded() const override;
	virtual void Load() override;
	virtual void Unload() override;

	virtual bool IsLoadDataValid() { return true; }						// Optional public override for derived classes

	void SetTint(float fR, float fG, float fB);
	void SetTint(uint32 uiColor);

	float CalculateAlpha();
	const glm::vec3 &CalculateTopTint();
	const glm::vec3 &CalculateBotTint();

	bool IsScissorSet() const;
	void GetLocalScissor(HyScreenRect<int32> &scissorOut) const;
	void GetWorldScissor(HyScreenRect<int32> &scissorOut);
	virtual void SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight);
	virtual void ClearScissor(bool bUseParentScissor);

	bool IsStencilSet() const;
	HyStencil *GetStencil() const;
	virtual void SetStencil(HyStencil *pStencil);
	virtual void ClearStencil(bool bUseParentStencil);

	int32 GetCoordinateSystem() const;
	virtual void UseCameraCoordinates();
	virtual void UseWindowCoordinates(int32 iWindowIndex = 0);

	int32 GetDisplayOrder() const;
	virtual void SetDisplayOrder(int32 iOrderValue);

protected:
	const IHyNodeData *UncheckedGetData();								// Used internally when it's guaranteed that data has already been acquired for this instance

	// Optional overrides for derived classes
	virtual void DrawLoadedUpdate() { }			// Invoked once after OnLoaded(), then once every frame (guarenteed to only be invoked if this instance is loaded)
	virtual void OnDataAcquired() { }			// Invoked once on the first time this node's data is queried
	virtual void OnLoaded() { }					// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnUnloaded() { }				// HyAssets invokes this instance's data has been erased

protected:
	virtual void NodeUpdate() = 0;

	// Internal Entity propagation function overrides
	virtual void _SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding) override;
	virtual void _SetStencil(HyStencilHandle hHandle, bool bIsOverriding) override;
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) override;
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) override;

private:
	void CalculateColor();
};

#endif /* IHyNodeDraw2d_h__ */
