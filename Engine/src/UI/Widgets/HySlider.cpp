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
	m_Slider(this)
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
	m_Slider(this)
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
	m_Slider.Setup(sliderInitRef);
	
	if(m_eOrientation == HYORIEN_Horizontal)
		SetSizePolicy(HYSIZEPOLICY_Expanding, HYSIZEPOLICY_Fixed);
	else
		SetSizePolicy(HYSIZEPOLICY_Fixed, HYSIZEPOLICY_Expanding);
	
	Assemble();
}

/*virtual*/ glm::vec2 HySlider::GetPosOffset() /*override*/
{
	if(m_eOrientation == HYORIEN_Horizontal)
		return glm::vec2(GetBarRadius(), m_Slider.size.GetAnimFloat(HYORIEN_Vertical).Get() * 0.5f);
	else
		return glm::vec2(m_Slider.size.GetAnimFloat(HYORIEN_Horizontal).Get() * 0.5f, GetBarRadius());
}

/*virtual*/ void HySlider::OnSetSizeHint() /*override*/
{
	float fRadius = GetBarRadius();

	if(m_eOrientation == HYORIEN_Horizontal)
		HySetVec(m_vSizeHint, static_cast<int32>(m_fLength + (fRadius * 2)), static_cast<int32>(m_Slider.size.GetAnimFloat(HYORIEN_Vertical).Get()));
	else
		HySetVec(m_vSizeHint, static_cast<int32>(m_Slider.size.GetAnimFloat(HYORIEN_Horizontal).Get()), static_cast<int32>(m_fLength + (fRadius * 2)));
}

/*virtual*/ glm::ivec2 HySlider::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	m_Slider.size.GetAnimFloat(m_eOrientation ^ 1) = static_cast<float>(uiNewHeight);
	m_fLength = uiNewWidth - (GetBarRadius() * 2);
	OnSetSizeHint();

	Assemble();
	return m_vSizeHint;
}

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

float HySlider::GetBarThickness()
{
	return m_Slider.size.GetAnimFloat(m_eOrientation ^ 1).Get() * 0.5f;
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
	m_Slider.pos.GetAnimFloat(m_eOrientation ^ 1) = fBarThickness * -0.5f;
	
	m_BarStroke.Assemble(m_eOrientation, fBarThickness, m_fLength, 0.0f);
	m_BarFill.Assemble(m_eOrientation, fBarThickness, m_fLength, m_fStrokeAmt);

	// Now position the slider on the bar at the proper location based on current values
	uint32 uiNumSteps = static_cast<uint32>(m_StepList.size());
	if(uiNumSteps == 0)
		uiNumSteps = abs(m_iMax - m_iMin) / m_uiStep;
	if(uiNumSteps == 0)
		uiNumSteps = 1;

	float fPos = (m_fLength / uiNumSteps) * (m_iValue - m_iMin);
	m_Slider.pos.GetAnimFloat(m_eOrientation) = fPos;
	
	m_BarStroke.m_BarPos.scale.GetAnimFloat(m_eOrientation) = fPos;
	m_BarStroke.m_BarNeg.pos.GetAnimFloat(m_eOrientation) = fPos;
	m_BarStroke.m_BarNeg.scale.GetAnimFloat(m_eOrientation) = m_fLength - fPos;

	m_BarFill.m_BarPos.scale.GetAnimFloat(m_eOrientation) = fPos;
	m_BarFill.m_BarNeg.pos.GetAnimFloat(m_eOrientation) = fPos;
	m_BarFill.m_BarNeg.scale.GetAnimFloat(m_eOrientation) = m_fLength - fPos;

	SetSizeAndLayoutDirty();
}
