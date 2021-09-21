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

class HyScrollBar : public HyEntity2d
{
	class Handle : public HyEntity2d
	{
	public:
		HyPrimitive2d			panel;

		Handle(uint32 uiDiameter, HyEntity2d *pParent) :
			HyEntity2d(pParent),
			panel(this)
		{
			EnableMouseInput();

			panel.SetAsBox(uiDiameter, uiDiameter);
		}

	protected:
		//virtual void OnUpdate() override;
		//virtual void OnMouseLeave() override;
		//virtual void OnMouseDown() override;
		//virtual void OnMouseClicked() override;
	};
	class Button : public HyButton
	{
		const HyOrientation		m_eORIENTATION;
		const bool				m_bPOSITIVE;

		HyPrimitive2d			panel;
		HyPrimitive2d			decal;

	public:
		Button(HyOrientation eOrientation, bool bPositive, uint32 uiDiameter, HyEntity2d *pParent) :
			HyButton(pParent),
			m_eORIENTATION(eOrientation),
			m_bPOSITIVE(bPositive),
			panel(this),
			decal(this)
		{
			SetDiameter(uiDiameter);
		}

		void SetDiameter(uint32 uiDiameter)
		{
			panel.SetAsBox(uiDiameter, uiDiameter);

			if(m_eORIENTATION == HYORIEN_Vertical)
			{
				if(m_bPOSITIVE)
				{
					glm::vec2 ptUpArrow[3];
					HySetVec(ptUpArrow[0], uiDiameter * 0.2f, uiDiameter * 0.2f);
					HySetVec(ptUpArrow[1], uiDiameter * 0.5f, uiDiameter * 0.8f);
					HySetVec(ptUpArrow[2], uiDiameter * 0.8f, uiDiameter * 0.2f);
					decal.SetAsPolygon(ptUpArrow, 3);
				}
				else // negative
				{
					glm::vec2 ptDownArrow[3];
					HySetVec(ptDownArrow[0], uiDiameter * 0.2f, uiDiameter * 0.8f);
					HySetVec(ptDownArrow[1], uiDiameter * 0.5f, uiDiameter * 0.2f);
					HySetVec(ptDownArrow[2], uiDiameter * 0.8f, uiDiameter * 0.8f);
					decal.SetAsPolygon(ptDownArrow, 3);
				}
			}
			else // HYORIEN_Horizontal
			{
				if(m_bPOSITIVE)
				{
					glm::vec2 ptRightArrow[3];
					HySetVec(ptRightArrow[0], uiDiameter * 0.2f, uiDiameter * 0.8f);
					HySetVec(ptRightArrow[1], uiDiameter * 0.8f, uiDiameter * 0.5f);
					HySetVec(ptRightArrow[2], uiDiameter * 0.2f, uiDiameter * 0.2f);
					decal.SetAsPolygon(ptRightArrow, 3);
				}
				else // negative
				{
					glm::vec2 ptLeftArrow[3];
					HySetVec(ptLeftArrow[0], uiDiameter * 0.2f, uiDiameter * 0.5f);
					HySetVec(ptLeftArrow[1], uiDiameter * 0.8f, uiDiameter * 0.8f);
					HySetVec(ptLeftArrow[2], uiDiameter * 0.8f, uiDiameter * 0.2f);
					decal.SetAsPolygon(ptLeftArrow, 3);
				}
			}
		}
	};

	const HyOrientation	m_eORIENTATION;

	HyPrimitive2d		m_Panel;
	Handle				m_Handle;
	Button				m_PosBtn;
	Button				m_NegBtn;

	bool				m_bIsValidMetrics;

public:
	HyScrollBar(HyOrientation eOrientation, uint32 uiDiameter, HyEntity2d *pParent);

	void SetMetrics(uint32 uiTotalLength, uint32 uiDiameter, uint32 uiClientTotalSize, uint32 uiClientShownSize);
	bool IsValidMetrics() const;
	static void OnArrowBtnPressed(HyButton *pBtn, void *pData);
};

#endif /* HyScrollBar_h__ */
