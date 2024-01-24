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
	friend class HyUiContainer;

protected:
	enum WidgetAttributes
	{
		UIATTRIB_HideDisabled = 1 << 0,		// Don't visually indicate if disabled
		UIATTRIB_IsDisabled = 1 << 1,
		UIATTRIB_KeyboardFocusAllowed = 1 << 2,
		UIATTRIB_IsKeyboardFocus = 1 << 3,

		UIATTRIB_FLAG_NEXT = 1 << 4,
	};
	uint32								m_uiAttribs;
	HyMouseCursor						m_eHoverCursor;					// When mouse hovers over *this, change to a specified cursor

public:
	IHyWidget(HyEntity2d *pParent = nullptr);
	virtual ~IHyWidget();

	bool IsInputAllowed() const;					// Checks itself and the container it's inserted in if input is allowed

	bool IsEnabled() const;
	virtual void SetAsEnabled(bool bEnabled);

	bool IsHideDisabled() const;					// Whether to not visually indicate if disabled
	void SetHideDisabled(bool bIsHideDisabled);		// Whether to not visually indicate if disabled

	bool IsKeyboardFocusAllowed() const;
	void SetKeyboardFocusAllowed(bool bEnabled);
	bool IsKeyboardFocus() const;
	bool RequestKeyboardFocus();

	bool IsHoverCursor() const;
	void SetHoverCursor(HyMouseCursor eMouseCursor);

protected:
	virtual void OnMouseEnter() override final;
	virtual void OnMouseLeave() override final;
	virtual void OnMouseDown() override final;
	virtual void OnMouseClicked() override final;

	virtual void OnUiTextInput(std::string sNewText) { }
	virtual void OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState) { }
	virtual void OnUiMouseEnter() { }
	virtual void OnUiMouseLeave() { }
	virtual void OnUiMouseDown() { }
	virtual void OnUiMouseClicked() { }

	void TakeKeyboardFocus();
	void RelinquishKeyboardFocus();
	virtual void OnTakeKeyboardFocus() { }			// Widgets that are 'IsKeyboardFocusAllowed' should override this
	virtual void OnRelinquishKeyboardFocus() { }	// Widgets that are 'IsKeyboardFocusAllowed' should override this
};

#endif /* IHyWidget_h__ */
