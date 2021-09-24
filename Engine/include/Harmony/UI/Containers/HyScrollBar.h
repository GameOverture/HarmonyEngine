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
typedef std::function<void(HyScrollBar *pSelf, uint32 uiNewPosition, void *pData)> HyScrollBarCallback;

class HyScrollBar : public HyEntity2d
{
	class PageControl : public HyEntity2d
	{
		HyPrimitive2d			m_Panel;

	public:
		PageControl(HyOrientation eOrientation, uint32 uiDiameter, uint32 uiLength, HyEntity2d *pParent) :
			HyEntity2d(pParent),
			m_Panel(this)
		{
			EnableMouseInput();
			SetMetrics(eOrientation, uiDiameter, uiLength);
		}

		void SetMetrics(HyOrientation eOrientation, uint32 uiDiameter, uint32 uiLength)
		{
			if(eOrientation == HYORIEN_Vertical)
				m_Panel.SetAsBox(uiDiameter, uiLength);
			else
				m_Panel.SetAsBox(uiLength, uiDiameter);
		}

		void SetColor(HyColor color)
		{
			m_Panel.SetTint(color);
		}
	};

	class Slider : public PageControl
	{
	public:
		Slider(HyOrientation eOrientation, uint32 uiDiameter, uint32 uiLength, HyEntity2d *pParent) :
			PageControl(eOrientation, uiDiameter, uiLength, pParent)
		{ }
	};

	class Button : public HyButton
	{
		HyPrimitive2d			m_Panel;
		HyPrimitive2d			m_Decal;

	public:
		Button(HyOrientation eOrientation, bool bPositive, uint32 uiDiameter, HyEntity2d *pParent) :
			HyButton(pParent),
			m_Panel(this),
			m_Decal(this)
		{
			SetMetrics(eOrientation, bPositive, uiDiameter);
		}

		void SetColor(HyColor color)
		{
			m_Panel.SetTint(color);
			if(color.IsLight())
				m_Decal.SetTint(color.Darken());
			else
				m_Decal.SetTint(color.Lighten());
		}

		void SetMetrics(HyOrientation eOrientation, bool bPositive, uint32 uiDiameter)
		{
			m_Panel.SetAsBox(uiDiameter, uiDiameter);

			if(eOrientation == HYORIEN_Vertical)
			{
				if(bPositive)
				{
					glm::vec2 ptUpArrow[3];
					HySetVec(ptUpArrow[0], uiDiameter * 0.2f, uiDiameter * 0.2f);
					HySetVec(ptUpArrow[1], uiDiameter * 0.5f, uiDiameter * 0.8f);
					HySetVec(ptUpArrow[2], uiDiameter * 0.8f, uiDiameter * 0.2f);
					m_Decal.SetAsPolygon(ptUpArrow, 3);
				}
				else // negative
				{
					glm::vec2 ptDownArrow[3];
					HySetVec(ptDownArrow[0], uiDiameter * 0.2f, uiDiameter * 0.8f);
					HySetVec(ptDownArrow[1], uiDiameter * 0.5f, uiDiameter * 0.2f);
					HySetVec(ptDownArrow[2], uiDiameter * 0.8f, uiDiameter * 0.8f);
					m_Decal.SetAsPolygon(ptDownArrow, 3);
				}
			}
			else // HYORIEN_Horizontal
			{
				if(bPositive)
				{
					glm::vec2 ptRightArrow[3];
					HySetVec(ptRightArrow[0], uiDiameter * 0.2f, uiDiameter * 0.8f);
					HySetVec(ptRightArrow[1], uiDiameter * 0.8f, uiDiameter * 0.5f);
					HySetVec(ptRightArrow[2], uiDiameter * 0.2f, uiDiameter * 0.2f);
					m_Decal.SetAsPolygon(ptRightArrow, 3);
				}
				else // negative
				{
					glm::vec2 ptLeftArrow[3];
					HySetVec(ptLeftArrow[0], uiDiameter * 0.2f, uiDiameter * 0.5f);
					HySetVec(ptLeftArrow[1], uiDiameter * 0.8f, uiDiameter * 0.8f);
					HySetVec(ptLeftArrow[2], uiDiameter * 0.8f, uiDiameter * 0.2f);
					m_Decal.SetAsPolygon(ptLeftArrow, 3);
				}
			}
		}
	};

	const HyOrientation	m_eORIENTATION;

	PageControl					m_PageControl;
	Slider						m_Slider;
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

	void SetColor(HyColor color);
	void SetMetrics(uint32 uiLength, uint32 uiDiameter, float fClientTotalSize, float fClientShownSize);
	bool IsValidMetrics() const;

	void SetOnScrollCallback(HyScrollBarCallback fpCallback, void *pData);

	void DoLineScroll(int32 iLinesOffset);
	void DoPageScroll(int32 iPagesOffset);

protected:
	virtual void OnUpdate() override;

	static void OnArrowBtnPressed(HyButton *pBtn, void *pData);

private:
	void InvokeOnScrollCallback();
};

#endif /* HyScrollBar_h__ */
