/**************************************************************************
*	HySlider.cpp
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HySlider.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "HyEngine.h"

void HySlider::BarPrimitives::Assemble(HyOrientation eOrientation, float fBarThickness, float fBarLength, float fIndentAmt)
{
	HyOrientation eInverseOrien = static_cast<HyOrientation>(eOrientation ^ 1);

	float fRadius = (fBarThickness * 0.5f) - fIndentAmt;
	m_EndCapNeg.pos.Set(0.0f, 0.0f);
	m_EndCapNeg.shape.SetAsCircle(fRadius);

	m_EndCapPos.pos.GetAnimFloat(eOrientation) = fBarLength;
	m_EndCapPos.pos.GetAnimFloat(eInverseOrien) = 0.0f;
	m_EndCapPos.shape.SetAsCircle(fRadius);

	if(eOrientation == HYORIEN_Horizontal)
	{
		m_BarPos.shape.SetAsBox(1.0f, fBarThickness - fIndentAmt);
		m_BarNeg.shape.SetAsBox(1.0f, fBarThickness - fIndentAmt);
	}
	else
	{
		m_BarPos.shape.SetAsBox(fBarThickness - fIndentAmt, 1.0f);
		m_BarNeg.shape.SetAsBox(fBarThickness - fIndentAmt, 1.0f);
	}

	m_BarPos.pos.GetAnimFloat(eOrientation) = 0.0f;
	m_BarPos.pos.GetAnimFloat(eInverseOrien) = fBarThickness * -0.5f;

	m_BarNeg.pos.GetAnimFloat(eOrientation) = 0.0f;
	m_BarNeg.pos.GetAnimFloat(eInverseOrien) = fBarThickness * -0.5f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HySlider::HySlider(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_iMin(0),
	m_iMax(100),
	m_uiStep(5),
	m_fLength(200.0f),
	m_fStrokeAmt(2.0f),
	m_iValue(0),
	m_eOrientation(HYORIEN_Horizontal),
	m_bInvertedAppearance(false),
	m_BarStroke(this),
	m_BarFill(this),
	m_Slider(this),
	m_bIsDragging(false),
	m_ptSliderCenter(0.0f, 0.0f),
	m_fpOnValueChanged(nullptr),
	m_pValueChangedParam(nullptr)
{
}

HySlider::HySlider(const HyPanelInit &sliderInitRef, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_iMin(0),
	m_iMax(100),
	m_uiStep(5),
	m_fLength(200.0f),
	m_fStrokeAmt(2.0f),
	m_iValue(0),
	m_eOrientation(HYORIEN_Horizontal),
	m_bInvertedAppearance(false),
	m_BarStroke(this),
	m_BarFill(this),
	m_Slider(this),
	m_bIsDragging(false),
	m_ptSliderCenter(0.0f, 0.0f),
	m_fpOnValueChanged(nullptr),
	m_pValueChangedParam(nullptr)
{
	Setup(sliderInitRef);
}

/*virtual*/ HySlider::~HySlider()
{
}

/*virtual*/ float HySlider::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	OnSetSizeHint();
	return m_vSizeHint.x * fPercent;
}

/*virtual*/ float HySlider::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	OnSetSizeHint();
	return m_vSizeHint.y * fPercent;
}

void HySlider::Setup(const HyPanelInit &sliderInitRef)
{
	//if(sliderInitRef.m_PanelColor.
	m_Slider.Setup(sliderInitRef, false);
	SetBarColors();

	SetAsEnabled(IsEnabled());
	
	if(m_eOrientation == HYORIEN_Horizontal)
		SetSizePolicy(HYSIZEPOLICY_Expanding, HYSIZEPOLICY_Fixed);
	else
		SetSizePolicy(HYSIZEPOLICY_Fixed, HYSIZEPOLICY_Expanding);
	
	Assemble();
}

uint32 HySlider::GetNumTicks() const
{
	uint32 uiNumSteps = static_cast<uint32>(m_StepList.size());
	if(uiNumSteps == 0)
		uiNumSteps = abs(m_iMax - m_iMin) / m_uiStep;
	if(uiNumSteps == 0)
		uiNumSteps = 1;

	return uiNumSteps;
}

int32 HySlider::GetValue() const
{
	return m_iValue;
}

void HySlider::SetValue(int32 iValue)
{
	iValue = HyClamp(iValue, m_iMin, m_iMax);
	if(m_iValue == iValue)
		return;

	m_iValue = iValue;
	PositionSlider();

	if(m_fpOnValueChanged)
		m_fpOnValueChanged(this, m_pValueChangedParam);
}

int32 HySlider::GetMin() const
{
	return m_iMin;
}

int32 HySlider::GetMax() const
{
	return m_iMax;
}

// If max is smaller than min, min becomes the only legal value.
void HySlider::SetRange(int32 iMin, int32 iMax)
{
	if(iMax < iMin)
		m_iMin = m_iMax = iMin;
	else
	{
		m_iMin = iMin;
		m_iMax = iMax;
	}

	m_iValue = HyClamp(m_iValue, m_iMin, m_iMax);
	PositionSlider();
}

uint32 HySlider::GetStep() const
{
	return m_uiStep;
}

void HySlider::SetStep(uint32 uiStepAmt)
{
	m_uiStep = uiStepAmt;
	PositionSlider();
}

void HySlider::SetSliderColors(HyColor panelColor /*= HyColor::WidgetPanel*/, HyColor frameColor /*= HyColor::WidgetFrame*/)
{
	m_Slider.SetPanelColor(panelColor);
	m_Slider.SetFrameColor(frameColor);
}

void HySlider::SetBarColors(HyColor posColor /*= HyColor::Blue*/, HyColor negColor /*= HyColor::Gray*/, HyColor strokeColor /*= HyColor::Black*/)
{
	m_BarStroke.SetTint(strokeColor);

	m_BarFill.m_EndCapPos.SetTint(negColor, negColor.Darken());
	m_BarFill.m_BarPos.SetTint(posColor, posColor.Darken());
	m_BarFill.m_EndCapNeg.SetTint(posColor, posColor.Darken());
	m_BarFill.m_BarNeg.SetTint(negColor, negColor.Darken());
}

void HySlider::SetValueChangedCallback(std::function<void(HySlider *, void *)> fpCallback, void *pParam /*= nullptr*/)
{
	m_fpOnValueChanged = fpCallback;
	m_pValueChangedParam = pParam;
}

/*virtual*/ void HySlider::OnUpdate() /*override*/
{
	if(m_bIsDragging)
	{
		if(HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft) == false)
		{
			m_bIsDragging = false;
			return;
		}

		glm::vec2 ptMousePos;
		if(GetCoordinateSystem() >= 0)
		{
			if(HyEngine::Input().GetMouseWindowIndex() == GetCoordinateSystem())
				ptMousePos = HyEngine::Input().GetMousePos();
			else
				return;
		}
		else
		{
			if(HyEngine::Input().GetWorldMousePos(ptMousePos) == false)
				return;
		}

		// How many ticks has been dragged
		float fTickSpacing = m_fLength / GetNumTicks();
		glm::vec2 vDist = ptMousePos - m_ptSliderCenter;
		int32 iNumThresholds = static_cast<int32>(vDist[m_eOrientation] / fTickSpacing);
		if(iNumThresholds != 0)
			SetValue(m_iValue + (iNumThresholds * m_uiStep));
	}
}

/*virtual*/ glm::vec2 HySlider::GetPosOffset() /*override*/
{
	if(m_eOrientation == HYORIEN_Horizontal)
		return glm::vec2(m_Slider.size.GetAnimFloat(HYORIEN_Horizontal).Get() * 0.5f, m_Slider.size.GetAnimFloat(HYORIEN_Vertical).Get() * 0.5f);
	else
		return glm::vec2(m_Slider.size.GetAnimFloat(HYORIEN_Horizontal).Get() * 0.5f, m_Slider.size.GetAnimFloat(HYORIEN_Vertical).Get() * 0.5f);
}

/*virtual*/ void HySlider::OnSetSizeHint() /*override*/
{
	float fRadius = GetBarRadius();

	if(m_eOrientation == HYORIEN_Horizontal)
		HySetVec(m_vSizeHint, static_cast<int32>(m_fLength + m_Slider.size.X()), static_cast<int32>(m_Slider.size.Y()));
	else
		HySetVec(m_vSizeHint, static_cast<int32>(m_Slider.size.X()), static_cast<int32>(m_fLength + m_Slider.size.Y()));
}

/*virtual*/ glm::ivec2 HySlider::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	m_Slider.size.GetAnimFloat(m_eOrientation ^ 1) = static_cast<float>(uiNewHeight);
	m_fLength = uiNewWidth - (m_Slider.size.GetAnimFloat(m_eOrientation).Get());
	OnSetSizeHint();

	Assemble();
	return m_vSizeHint;
}

/*virtual*/ void HySlider::OnUiMouseDown() /*override*/
{
	m_bIsDragging = true;
}

float HySlider::GetBarThickness()
{
	return m_Slider.size.GetAnimFloat(m_eOrientation ^ 1).Get() * 0.25f;
}

float HySlider::GetBarRadius()
{
	return GetBarThickness() * 0.5f;
}

void HySlider::Assemble()
{
	if(m_Slider.IsValid() == false)
		return;

	// Build the slider bar out out of primitives
	float fBarThickness = GetBarThickness();
	m_Slider.pos.GetAnimFloat(m_eOrientation ^ 1) = m_Slider.size.GetAnimFloat(m_eOrientation ^ 1).Get() * -0.5f;
	
	m_BarStroke.Assemble(m_eOrientation, fBarThickness, m_fLength, 0.0f);
	m_BarFill.Assemble(m_eOrientation, fBarThickness, m_fLength, m_fStrokeAmt);

	// Now position the slider on the bar at the proper location based on current values
	PositionSlider();
}

void HySlider::PositionSlider()
{
	uint32 uiNumTicks = GetNumTicks();

	float fPos = (m_fLength / uiNumTicks) * ((m_iValue - m_iMin) / m_uiStep);
	m_Slider.pos.GetAnimFloat(m_eOrientation) = fPos - (m_Slider.size.GetAnimFloat(m_eOrientation).Get() * 0.5f);

	auto ptCenter = m_Slider.GetSceneAABB().GetCenter();
	HySetVec(m_ptSliderCenter, ptCenter.x, ptCenter.y);

	m_BarStroke.m_BarPos.scale.GetAnimFloat(m_eOrientation) = fPos;
	m_BarStroke.m_BarNeg.pos.GetAnimFloat(m_eOrientation) = fPos;
	m_BarStroke.m_BarNeg.scale.GetAnimFloat(m_eOrientation) = m_fLength - fPos;

	m_BarFill.m_BarPos.scale.GetAnimFloat(m_eOrientation) = fPos;
	m_BarFill.m_BarNeg.pos.GetAnimFloat(m_eOrientation) = fPos;
	m_BarFill.m_BarNeg.scale.GetAnimFloat(m_eOrientation) = m_fLength - fPos;

	SetSizeAndLayoutDirty();
}
