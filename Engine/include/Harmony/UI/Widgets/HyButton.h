/**************************************************************************
*	HyButton.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyButton_h__
#define HyButton_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"

class HyButton;
typedef std::function<void(HyButton *, void *)> HyButtonClickedCallback;

enum HyButtonState
{
	HYBUTTONSTATE_Idle = 0,
	HYBUTTONSTATE_Down,
	HYBUTTONSTATE_Highlighted,
	HYBUTTONSTATE_HighlightedDown,
	HYBUTTONSTATE_Hover,
	HYBUTTONSTATE_HighlightedHover
};

class HyButton : public HyLabel
{
protected:
	HyButtonClickedCallback		m_fpBtnClickedCallback;
	void *						m_pBtnClickedParam;

public:
	HyButton(HyEntity2d *pParent = nullptr);
	HyButton(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyButton(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	virtual ~HyButton();

	virtual void SetAsDisabled(bool bIsDisabled) override;
	virtual void SetAsHighlighted(bool bIsHighlighted) override;

	bool IsHideDownState() const;
	void SetHideDownState(bool bIsHideDownState);

	bool IsHideHoverState() const;
	void SetHideHoverState(bool bIsHideHoverState);

	void SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam = nullptr);
	void InvokeButtonClicked();

protected:
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	virtual void OnMouseDown() override;
	virtual void OnMouseUp() override;
	virtual void OnMouseClicked() override;
};

#endif /* HyButton_h__ */
