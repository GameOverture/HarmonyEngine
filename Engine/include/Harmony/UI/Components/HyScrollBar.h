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
#include "UI/Widgets/HyButton.h"

class HyScrollBar;
typedef std::function<void(HyScrollBar *pSelf, float fNewPosition, float fTotalRange, void *pData)> HyScrollBarCallback;

class HyScrollBar : public HyEntity2d
{
	class PageControl : public HyEntity2d
	{
		uint32					m_uiDiameter;

		HyPrimitive2d			m_Panel;
		HyPrimitive2d			m_Slider;

		enum DragState
		{
			DRAG_None = 0,
			DRAG_MouseHeld,
			DRAG_MouseDragCheck,
			DRAG_IsDragging
		};
		DragState				m_eDragState;
		glm::vec2				m_ptDragPos;

	public:
		PageControl(HyOrientation eOrientation, uint32 uiLength, uint32 uiDiameter, HyEntity2d *pParent);

		uint32 GetDiameter() const;

		void SetMetrics(HyOrientation eOrientation, uint32 uiLength, uint32 uiDiameter, float fSliderPercent);
		void SetSliderPos(HyOrientation eOrientation, float fAnimScrollPos, float fClientTotalSize, float fClientShownSize);
		void SetColor(HyColor color);

	protected:
		virtual void OnUpdate() override;
		virtual void OnMouseDown() override;
		virtual void OnMouseClicked() override;
	};
	class Button : public HyButton
	{
		HyPrimitive2d			m_Panel;
		HyPrimitive2d			m_Decal;

	public:
		Button(HyOrientation eOrientation, bool bPositive, uint32 uiDiameter, HyEntity2d *pParent);

		void SetColor(HyColor color);
		void SetMetrics(HyOrientation eOrientation, bool bPositive, uint32 uiDiameter);
	};

	const HyOrientation			m_eORIENTATION;

	PageControl					m_PageControl;
	Button						m_PosBtn;
	Button						m_NegBtn;
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

	HyOrientation GetOrientation() const;

	uint32 GetDiameter() const;

	float GetLineScrollAmt() const;
	void SetLineScrollAmt(float fLineScrollAmt);

	void SetColor(HyColor color);
	void SetMetrics(uint32 uiLength, uint32 uiClientTotalSize, uint32 uiClientShownSize);
	bool IsValidMetrics() const;

	void SetOnScrollCallback(HyScrollBarCallback fpCallback, void *pData);

	void DoLineScroll(int32 iLinesOffset);
	void DoPageScroll(int32 iPagesOffset);

	void OffsetSlider(float fPixels);

protected:
	virtual void OnUpdate() override;

	static void OnArrowBtnPressed(HyButton *pBtn, void *pData);

private:
	void InvokeOnScrollCallback();
};

#endif /* HyScrollBar_h__ */
