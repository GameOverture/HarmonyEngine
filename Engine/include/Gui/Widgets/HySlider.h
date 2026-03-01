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
#include "Gui/Widgets/IHyWidget.h"
#include "Gui/Components/HyPanel.h"

class HySlider : public IHyWidget
{
protected:
	enum SliderAttributes
	{
		SLIDERATTRIB_UseStepList			= 1 << 16,
		SLIDERATTRIB_IsVertical				= 1 << 17,		// When 'SLIDERATTRIB_IsVertical' enabled, slider moves up/down instead of left/right
		SLIDERATTRIB_InvertedAppearance		= 1 << 18,		// When 'SLIDERATTRIB_InvertedAppearance' is false (the default), the minimum and maximum along the slider will be shown in its classic position
		SLIDERATTRIB_IsDragging				= 1 << 19,		// When 'SLIDERATTRIB_IsDragging' enabled, the user is currently dragging the slider handle

		SLIDERATTRIB_NEXTFLAG				= 1 << 20
	};
	static_assert((int)SLIDERATTRIB_UseStepList == (int)WIDGETATTRIB_NEXTFLAG, "HySlider is not matching with base classes attrib flags");

	float									m_fBarSize;
	float									m_fBarThickness;
	float									m_fHandleSize;
	float									m_fHandleThickness;

	int64									m_iMin;
	int64									m_iMax;
	int32									m_iStep;
	std::vector<int64>						m_StepList;			// When used, only values present in this std::vector can be selected
	int64									m_iValue;

	//struct BarPrimitives : public HyEntity2d
	//{
	//	HyPrimitive2d						m_EndCapNeg;
	//	HyPrimitive2d						m_EndCapPos;
	//	HyPrimitive2d						m_BarPos;		// Colored side of the slider
	//	HyPrimitive2d						m_BarNeg;		// Dimmed side of the slider

	//	BarPrimitives(HyEntity2d *pParent);
	//	void DoAssembly(HyOrientation eOrientation, float fBarThickness, float fBarLength, float fIndentAmt);
	//};
	//BarPrimitives							m_BarStroke;
	//BarPrimitives							m_BarFill;

	std::function<void(HySlider *)>			m_fpOnValueChanged;

public:
	HyPanel									bar;
	HyPanel									handle;

public:
	HySlider(HyEntity2d *pParent = nullptr);
	HySlider(HyOrientation eOrien, const HyUiPanelInit &barInitRef, const HyUiPanelInit &handleInitRef, HyEntity2d *pParent = nullptr);
	virtual ~HySlider();

	void Setup(HyOrientation eOrien, const HyUiPanelInit &barInitRef, const HyUiPanelInit &handleInitRef);

	int64 GetNumTicks() const;
	int64 GetValue() const;
	void SetValue(int64 iValue);

	int64 GetMin() const;
	int64 GetMax() const;
	void SetRange(int64 iMin, int64 iMax, int32 iStepAmt); // If iMax < iMin, iMin becomes the only legal value. An invalid 'iStepAmt' will become 1
	void SetRange(const std::vector<int64> &stepList); // An empty stepList is ignored

	HyOrientation GetOrientation() const;
	void SetOrientation(HyOrientation eOrien);

	void SetValueChangedCallback(std::function<void(HySlider *)> fpCallback);

protected:
	virtual void Update() override;
	virtual void OnAssemble() override;

	virtual glm::ivec2 OnCalcPreferredSize() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	virtual void OnUiMouseDown() override;

	virtual void OnSetup() { }					// Optional override for derived classes

	float GetBarThickness();
	float GetBarRadius();

	void PositionHandle();
};

#endif /* HySlider_h__ */
