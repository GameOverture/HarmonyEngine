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

class HyLayout;

class IHyEntityUi : public HyEntity2d
{
	friend class HyUiContainer;
	friend class HyScrollContainer;
	friend class HyLayout;

protected:
	HySizePolicy						m_SizePolicies[HYNUM_ORIENTATIONS];
	glm::ivec2							m_vMinSize;
	glm::ivec2							m_vMaxSize;
	bool								m_bLockProportions;

	bool								m_bSizeHintDirty;
	glm::ivec2							m_vSizeHint;

public:
	IHyEntityUi(HyEntity2d *pParent = nullptr);
	virtual ~IHyEntityUi();

	HySizePolicy GetHorizontalPolicy();
	HySizePolicy GetVerticalPolicy();
	virtual HySizePolicy GetSizePolicy(HyOrientation eOrien);

	void SetSizePolicy(HySizePolicy eHorizPolicy, HySizePolicy eVertPolicy);
	void SetHorizontalPolicy(HySizePolicy ePolicy);
	void SetVerticalPolicy(HySizePolicy ePolicy);

	bool IsLockedProportions() const;
	void SetLockedProportions(bool bLockProportions);

	glm::ivec2 GetMinSize();
	void SetMinSize(uint32 uiMinSizeX, uint32 uiMinSizeY);
	glm::ivec2 GetMaxSize();
	void SetMaxSize(uint32 uiMaxSizeX, uint32 uiMaxSizeY);

	glm::ivec2 GetSizeHint();

protected:
	glm::ivec2 Resize(uint32 uiNewWidth, uint32 uiNewHeight);

	virtual glm::vec2 GetPosOffset() = 0;									// What offset is needed to get *this oriented to its bottom left
	virtual void OnSetSizeHint() = 0;										// Sets the preferred size of *this into 'm_vSizeHint'
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) = 0;	// Returns the size *this was actually set to (which may be different than what's passed in due to constraints)

	void SetSizeAndLayoutDirty();
};

#endif /* IHyEntityUi_h__ */
