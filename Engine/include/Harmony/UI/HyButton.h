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
#include "UI/HyInfoPanel.h"

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

class HyButton : public HyInfoPanel
{
protected:
	HyButtonClickedCallback		m_fpButtonClickedCallback;
	void *						m_pParam;

	bool						m_bAllowDownState;
	bool						m_bAllowHoverState;
	
	bool						m_bIsHighlighted;

public:
	HyButton(HyEntity2d *pParent = nullptr);
	HyButton(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyButton(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	virtual ~HyButton();

	virtual void SetAsDisabled(bool bIsDisabled) override;

	bool IsHighlighted() const;
	void SetAsHighlighted(bool bIsHighlighted);

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
