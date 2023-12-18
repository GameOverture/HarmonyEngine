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
protected:
	enum SliderAttributes
	{
		SLIDERATTRIB_UseStepList = 1 << 4,
		SLIDERATTRIB_IsVertical = 1 << 5,			// When 'SLIDERATTRIB_IsVertical' enabled, slider moves up/down instead of left/right
		SLIDERATTRIB_InvertedAppearance = 1 << 6,	// When 'SLIDERATTRIB_InvertedAppearance' is false (the default), the minimum and maximum along the slider will be shown in its classic position
		SLIDERATTRIB_IsDragging = 1 << 7,			// When 'SLIDERATTRIB_IsDragging' enabled, the user is currently dragging the slider handle

		SLIDERATTRIB_FLAG_NEXT = 1 << 8
	};
	static_assert((int)SLIDERATTRIB_UseStepList == (int)UIATTRIB_FLAG_NEXT, "HySlider is not matching with base classes attrib flags");

	int64									m_iMin;
	int64									m_iMax;
	uint32									m_uiStep;
	std::vector<int64>						m_StepList;				// When used, only values present in this std::vector can be selected
	float									m_fLength;				// Length of the slider bar in pixels (does not include the radius of the rounded ends)
	float									m_fStrokeAmt;			// The stroke amount in pixels when rendering the bar

	int64									m_iValue;

	struct BarPrimitives : public HyEntity2d
	{
		HyPrimitive2d						m_EndCapNeg;
		HyPrimitive2d						m_EndCapPos;
		HyPrimitive2d						m_BarPos;				// Colored side of the slider
		HyPrimitive2d						m_BarNeg;				// Dimmed side of the slider

		BarPrimitives(HyEntity2d *pParent);
		void Assemble(HyOrientation eOrientation, float fBarThickness, float fBarLength, float fIndentAmt);
	};
	BarPrimitives							m_BarStroke;
	BarPrimitives							m_BarFill;

	HyPanel									m_Slider;
	glm::vec2								m_ptSliderCenter;

	std::function<void(HySlider *, void *)>	m_fpOnValueChanged;
	void *									m_pValueChangedParam;

public:
	HySlider(HyEntity2d *pParent = nullptr);
	HySlider(const HyPanelInit &sliderInitRef, HyEntity2d *pParent = nullptr);
	virtual ~HySlider();

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	void Setup(const HyPanelInit &sliderInitRef);

	int64 GetNumTicks() const;
	int64 GetValue() const;
	void SetValue(int64 iValue);

	int64 GetMin() const;
	int64 GetMax() const;
	void SetRange(int64 iMin, int64 iMax, uint32 uiStepAmt); // If iMax < iMin, iMin becomes the only legal value. An invalid 'uiStepAmt' will become 1
	void SetRange(const std::vector<int64> &stepList); // An empty stepList is ignored

	HyOrientation GetOrientation() const;
	void SetOrientation(HyOrientation eOrien);

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
	void FixValues();
};

#endif /* HySlider_h__ */
