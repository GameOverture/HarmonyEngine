/**************************************************************************
*	IHyEntityUi.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyEntityUi_h__
#define IHyEntityUi_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Input/HyInputKeys.h"

class HyPanel;

class IHyEntityUi : public HyEntity2d
{
protected:
	glm::ivec2							m_vActualSize;

private:
	glm::ivec2							m_vMinSize;
	glm::ivec2							m_vMaxSize;

	glm::ivec2							m_vSizeHint;	// Ideal size. If a dimension is '0' then autosize it
	bool								m_bSizeDirty;
	bool								m_bLockProportions;
	HySizePolicy						m_SizePolicies[HYNUM_ORIENTATIONS];

public:
	IHyEntityUi(HyEntity2d *pParent = nullptr);
	virtual ~IHyEntityUi();

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;
	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;
	float GetSizeDimension(int32 iDimensionIndex, float fPercent = 1.0f);
	glm::ivec2 SetSizeDimension(int32 iDimensionIndex, uint32 uiSizeHint);

	HySizePolicy GetHorizontalPolicy() const;
	HySizePolicy GetVerticalPolicy() const;
	virtual HySizePolicy GetSizePolicy(HyOrientation eOrien) const;

	void SetSizePolicy(HySizePolicy eHorizPolicy, HySizePolicy eVertPolicy);
	void SetHorizontalPolicy(HySizePolicy ePolicy);
	void SetVerticalPolicy(HySizePolicy ePolicy);

	bool IsLockedProportions() const;
	void SetLockedProportions(bool bLockProportions);

	glm::ivec2 GetMinSize();
	void SetMinSize(uint32 uiMinSizeX, uint32 uiMinSizeY);
	glm::ivec2 GetMaxSize();
	void SetMaxSize(uint32 uiMaxSizeX, uint32 uiMaxSizeY);

	bool IsSizeDirty() const;
	glm::ivec2 GetPreferredSize();											// Ideal size. If a dimension is '0' then autosize it
	glm::ivec2 Resize(uint32 uiNewWidth, uint32 uiNewHeight);

	virtual glm::vec2 GetBotLeftOffset() = 0;								// What offset is needed to get *this oriented to its bottom left

protected:
	void SetSizeDirty();

	virtual glm::ivec2 OnCalcPreferredSize() = 0;							// Returns the preferred size (may also set Min/Max sizes). Returned result is set to 'm_vSizeHint'.
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) = 0;	// Returns the size *this was actually set to (which may be different than what's passed in due to constraints)
};

#endif /* IHyEntityUi_h__ */
