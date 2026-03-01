/**************************************************************************
*	HyScrollBar.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyScrollBar_h__
#define HyScrollBar_h__

#include "Afx/HyStdAfx.h"
#include "Gui/Widgets/HyButton.h"

class HyScrollBar;
typedef std::function<void(HyScrollBar *pSelf, float fNewPosition, float fTotalRange, void *pData)> HyScrollBarCallback;

class HyScrollBar : public HyEntity2d
{
	//class PageControl : public HyEntity2d
	//{
	//	uint32					m_uiDiameter;

	//	bool					m_bIsDragging;
	//	glm::vec2				m_ptDragPos;

	//public:
	//	PageControl(HyOrientation eOrientation, uint32 uiLength, uint32 uiDiameter, HyEntity2d *pParent);
	//	virtual ~PageControl();

	//	uint32 GetDiameter() const;
	//	void SetDiameter(uint32 uiDiameter);

	//	void SetMetrics(HyOrientation eOrientation, uint32 uiLength, uint32 uiDiameter, float fSliderPercent);
	//	void SetSliderPos(HyOrientation eOrientation, float fAnimScrollPos, float fClientTotalSize, float fClientShownSize);
	//	void SetColor(HyColor color);

	//protected:
	//	virtual void OnUpdate() override;
	//	virtual void OnMouseDown() override;
	//	virtual void OnMouseClicked() override;
	//};
	//class Button : public HyButton
	//{
	//	//HyPrimitive2d			m_Panel;
	//	//HyPrimitive2d			m_Decal;

	//public:
	//	Button(HyOrientation eOrientation, bool bPositive, uint32 uiDiameter, HyEntity2d *pParent);
	//	virtual ~Button();

	//	void SetColor(HyColor color);
	//	void SetMetrics(HyOrientation eOrientation, bool bPositive, uint32 uiDiameter);
	//};

	HyOrientation				m_eOrientation;

	uint32						m_uiDiameter;
	bool						m_bIsDragging;
	glm::vec2					m_ptDragPos;

	HyPanel						m_BarPanel;
	HyPanel						m_HandlePanel;
	//PageControl					m_PageControl;
	HyButton					m_PosBtn;
	HyButton					m_NegBtn;
	float						m_fLineScrollAmt;

	bool						m_bIsValidMetrics;

	float						m_fScrollPos;
	HyAnimFloat					m_AnimScrollPos;

	float						m_fClientTotalSize;
	float						m_fClientShownSize;

	HyScrollBarCallback			m_fpCallback;
	void *						m_pCallbackData;

public:
	HyScrollBar(HyOrientation eOrientation, uint32 uiDiameter, HyEntity2d *pParent);
	HyScrollBar(HyOrientation eOrientation, const HyUiPanelInit &posBtnInit, const HyUiPanelInit &negBtnInit, const HyUiPanelInit &barInit, const HyUiPanelInit &handleInit, HyEntity2d *pParent);
	virtual ~HyScrollBar();

	void Setup(HyOrientation eOrientation, uint32 uiDiameter);
	void Setup(HyOrientation eOrientation, const HyUiPanelInit &posBtnInit, const HyUiPanelInit &negBtnInit, const HyUiPanelInit &barInit, const HyUiPanelInit &handleInit);

	HyOrientation GetOrientation() const;

	uint32 GetDiameter() const;

	float GetLineScrollAmt() const;
	void SetLineScrollAmt(float fLineScrollAmt);

	//void SetColor(HyColor color);
	//void SetDiameter(uint32 uiDiameter);
	void SetMetrics(uint32 uiLength, uint32 uiClientTotalSize, uint32 uiClientShownSize);
	bool IsValidMetrics() const;

	void SetOnScrollCallback(HyScrollBarCallback fpCallback, void *pData);

	void DoLineScroll(int32 iLinesOffset);
	void DoPageScroll(int32 iPagesOffset);

	void OffsetSlider(float fPixels, bool bOffsetClientContentPixels);
	void ScrollTo(float fPos);

protected:
	virtual void OnUpdate() override;
	virtual void OnAssemble() override;

	virtual void OnMouseDown() override;

private:
	void InvokeOnScrollCallback();
};

#endif /* HyScrollBar_h__ */
