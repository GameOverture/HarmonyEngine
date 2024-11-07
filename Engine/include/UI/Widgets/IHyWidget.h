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
#include "UI/Components/HyPanel.h"

class IHyWidget : public IHyEntityUi
{
	friend class HyUiContainer;

protected:
	enum WidgetAttributes
	{
		WIDGETATTRIB_HideDisabled			= 1 << 7,		// Don't visually indicate if disabled
		WIDGETATTRIB_IsDisabled				= 1 << 8,		// When this widget is disabled

		WIDGETATTRIB_HideMouseHoverState	= 1 << 9,		// Don't visually indicate mouse hover state (when available)
		WIDGETATTRIB_HideDownState			= 1 << 10,		// Don't visually indicate down state (when available)

		WIDGETATTRIB_KeyboardFocusAllowed	= 1 << 11,		// Allow this widget to be the target of keyboard input
		WIDGETATTRIB_IsKeyboardFocus		= 1 << 12,		// When this widget will take keyboard input

		WIDGETATTRIB_HideHighlightedState	= 1 << 13,		// Don't visually indicate highlighted state (when available)
		WIDGETATTRIB_IsHighlighted			= 1 << 14,		// Indicates keyboard focus, or as an optional cosmetic state

		WIDGETATTRIB_IsTypeButton			= 1 << 15,		// Is ON when this widget is a HyButton (mainly used to distinguish for checkbox groups)

		WIDGETATTRIB_NEXTFLAG				= 1 << 16,
	};
	static_assert((int)WIDGETATTRIB_HideDisabled == (int)ENTITYATTRIB_NEXTFLAG, "IHyWidget is not matching with base classes attrib flags");

	HyMouseCursor							m_eHoverCursor;	// When mouse hovers over *this, change to a specified cursor
	HyPanelState							m_ePanelState;	// When 'WIDGETATTRIB_IsCustomPanelState' is NOT enabled, this is the current state of the panel
	HyPanel									m_Panel;		// A rectangular width/height that is typically a visible graphic background (or main part) of the widget

public:
	IHyWidget(HyEntity2d *pParent = nullptr);
	virtual ~IHyWidget();

	bool IsButton() const;

	virtual bool SetState(uint32 uiStateIndex) override;	// If used, this turns off using "panel states"
	virtual uint32 GetNumStates() override;

	HyPanelState GetPanelState() const;
	bool IsUsingPanelStates() const;
	void UsePanelStates();

	bool IsPanelVisible() const;
	void SetPanelVisible(bool bVisible);

	bool IsPanelBoundingVolume() const;
	bool IsPanelNode() const;
	IHyBody2d *GetPanelNode();

	bool IsInputAllowed() const;							// Checks itself and the container it's inserted in if input is allowed

	bool IsEnabled() const;
	bool IsHideDisabled() const;							// Whether to not visually indicate if disabled
	void SetHideDisabled(bool bIsHideDisabled);				// Whether to not visually indicate if disabled
	void SetAsEnabled(bool bEnabled);

	bool IsKeyboardFocus() const;
	bool IsKeyboardFocusAllowed() const;
	void SetKeyboardFocusAllowed(bool bEnabled);
	bool RequestKeyboardFocus();

	bool IsHideDownState() const;
	void SetHideDownState(bool bIsHideDownState);

	bool IsHideMouseHoverState() const;
	void SetHideMouseHoverState(bool bIsHideHoverState);
	bool IsMouseHoverCursorSet() const;
	void SetMouseHoverCursor(HyMouseCursor eMouseCursor);

	bool IsHighlighted() const;
	bool IsHideHighlightedState() const;
	void SetHideHighlightedState(bool bIsHideHighlightedState);
	void SetAsHighlighted(bool bIsHighlighted);

	virtual bool IsDepressed() const;				// Derived classes may override this if more than just mouse down is considered 'depressed' (e.g. button taking keyboard input like space/enter)

protected:
	virtual void OnMouseEnter() override final;
	virtual void OnMouseLeave() override final;
	virtual void OnMouseDown() override final;
	virtual void OnMouseUp() override final;
	virtual void OnMouseClicked() override final;

	virtual void OnSetSizeHint() override;			// Sets the preferred size of *this into 'm_vSizeHint

	virtual void OnUiTextInput(std::string sNewText) { }
	virtual void OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) { }
	virtual void OnUiMouseEnter() { }
	virtual void OnUiMouseLeave() { }
	virtual void OnUiMouseDown() { }
	virtual void OnUiMouseClicked() { }

	void TakeKeyboardFocus();
	void RelinquishKeyboardFocus();
	virtual void OnTakeKeyboardFocus() { }			// Widgets that are 'IsKeyboardFocusAllowed' should override this
	virtual void OnRelinquishKeyboardFocus() { }	// Widgets that are 'IsKeyboardFocusAllowed' should override this

	HyPanelState CalcPanelState();
	void ApplyPanelState();
	virtual void OnPanelUpdated() { }				// Invoked whenever m_Panel is modified
};

#endif /* IHyWidget_h__ */
