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
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"

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
	enum ButtonAttributes
	{
		BTNATTRIB_HideDownState		= 1 << 10,		// Don't visually indicate down state (when available)
		BTNATTRIB_HideHoverState	= 1 << 11,		// Don't visually indicate hover state (when available)
		BTNATTRIB_IsHighlighted		= 1 << 12,

		BTNATTRIB_FLAG_NEXT			= 1 << 13
	};
	static_assert(BTNATTRIB_HideDownState == LABELATTRIB_FLAG_NEXT, "HyButton is not matching with base classes attrib flags");

	HyButtonClickedCallback		m_fpBtnClickedCallback;
	void *						m_pBtnClickedParam;
	HyAudio2d					m_ClickedSound;

public:
	HyButton(HyEntity2d *pParent = nullptr);
	HyButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyButton();

	virtual void SetAsEnabled(bool bEnabled) override;

	bool IsHideDownState() const;
	void SetHideDownState(bool bIsHideDownState);

	bool IsHideHoverState() const;
	void SetHideHoverState(bool bIsHideHoverState);

	bool IsHighlighted() const;
	void SetAsHighlighted(bool bIsHighlighted);

	void SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam = nullptr, std::string sAudioPrefix = "", std::string sAudioName = "");
	void InvokeButtonClicked();

protected:
	virtual void OnSetup() override;

	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	virtual void OnMouseDown() override;
	virtual void OnMouseClicked() override;

	virtual void OnBtnStateChange(HyButtonState eNewState) { }

private:
	void SetBtnState(HyButtonState eState);
};

#endif /* HyButton_h__ */
