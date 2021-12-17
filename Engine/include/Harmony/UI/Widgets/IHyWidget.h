/**************************************************************************
*	IHyWidget.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyWidget_h__
#define IHyWidget_h__

#include "Afx/HyStdAfx.h"
#include "UI/IHyEntityUi.h"

class IHyWidget : public IHyEntityUi
{
	friend class HyContainer;

protected:
	enum WidgetAttributes
	{
		UIATTRIB_HideDisabled = 1 << 0,		// Don't visually indicate if disabled
		UIATTRIB_IsDisabled = 1 << 1,
		UIATTRIB_CanKeyboardFocus = 1 << 2,

		UIATTRIB_FLAG_NEXT = 1 << 3,
	};
	uint32								m_uiAttribs;
	HyMouseCursor						m_eHoverCursor;					// When mouse hovers over *this, change to a specified cursor

public:
	IHyWidget(HyEntity2d *pParent = nullptr);
	virtual ~IHyWidget();

	bool IsEnabled() const;
	virtual void SetAsEnabled(bool bEnabled);

	bool IsHideDisabled() const;				// Whether to not visually indicate if disabled
	void SetHideDisabled(bool bIsHideDisabled);	// Whether to not visually indicate if disabled

	bool IsKeyboardFocusAllowed() const;
	void SetKeyboardFocusAllowed(bool bEnabled);
	bool RequestKeyboardFocus();

	bool IsHoverCursor() const;
	void SetHoverCursor(HyMouseCursor eMouseCursor);

protected:
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	virtual void OnMouseClicked() override;

	virtual void OnTakeFocus() { }
	virtual void OnRelinquishFocus() { }
};

#endif /* IHyWidget_h__ */
