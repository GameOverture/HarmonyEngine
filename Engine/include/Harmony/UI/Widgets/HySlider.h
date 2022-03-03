/**************************************************************************
*	HySlider.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HySlider_h__
#define HySlider_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/IHyWidget.h"
#include "UI/Components/HyPanel.h"

class HySlider : public IHyWidget
{
	int32				m_iMin;
	int32				m_iMax;
	uint32				m_uiStep;
	std::vector<int32>	m_StepList;				// When used, only values present in this std::vector can be selected
	float				m_fLength;				// Length of the slider bar in pixels (does not include the radius of the rounded ends)
	float				m_fStrokeAmt;			// The stroke amount in pixels when rendering the bar

	int32				m_iValue;
	HyOrientation		m_eOrientation;
	bool				m_bInvertedAppearance;	// If this property is false (the default), the minimum and maximum along the slider will be shown in its classic position

	struct BarPrimitives : public HyEntity2d
	{
		HyPrimitive2d	m_EndCapNeg;
		HyPrimitive2d	m_EndCapPos;
		HyPrimitive2d	m_BarPos;				// Colored side of the slider
		HyPrimitive2d	m_BarNeg;				// Dimmed side of the slider

		BarPrimitives(HyEntity2d *pParent) :
			HyEntity2d(pParent),
			m_EndCapNeg(this),
			m_EndCapPos(this),
			m_BarPos(this),
			m_BarNeg(this)
		{ }

		void Assemble(HyOrientation eOrientation, float fBarThickness, float fBarLength, float fIndentAmt);
	};
	BarPrimitives		m_BarStroke;
	BarPrimitives		m_BarFill;

	HyPanel				m_Slider;
	bool				m_bIsDragging;
	glm::vec2			m_ptSliderCenter;

	std::function<void(HySlider *, void *)>		m_fpOnValueChanged;
	void *										m_pValueChangedParam;

public:
	HySlider(HyEntity2d *pParent = nullptr);
	HySlider(const HyPanelInit &sliderInitRef, HyEntity2d *pParent = nullptr);
	virtual ~HySlider();

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	void Setup(const HyPanelInit &sliderInitRef);

	uint32 GetNumTicks() const;
	int32 GetValue() const;
	void SetValue(int32 iValue);

	void SetSliderColors(HyColor panelColor = HyColor::WidgetPanel, HyColor frameColor = HyColor::WidgetFrame);
	void SetBarColors(HyColor posColor = HyColor::Blue, HyColor negColor = HyColor::Gray, HyColor strokeColor = HyColor::Black);

	void SetValueChangedCallback(std::function<void(HySlider *, void *)> fpCallback, void *pParam = nullptr);

protected:
	virtual void OnUpdate() override;

	virtual glm::vec2 GetPosOffset() override;
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	virtual void OnUiMouseDown() override;

	float GetBarThickness();
	float GetBarRadius();
	void Assemble();
	void PositionSlider();
};

#endif /* HySlider_h__ */
