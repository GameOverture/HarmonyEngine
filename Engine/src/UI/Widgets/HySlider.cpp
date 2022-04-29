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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BarPrimitives
HySlider::BarPrimitives::BarPrimitives(HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_EndCapNeg(this),
	m_EndCapPos(this),
	m_BarPos(this),
	m_BarNeg(this)
{ }
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
	m_BarStroke(this),
	m_BarFill(this),
	m_Slider(this),
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
	m_BarStroke(this),
	m_BarFill(this),
	m_Slider(this),
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
	
	Assemble();
}

uint32 HySlider::GetNumTicks() const
{
	if(m_uiAttribs & SLIDERATTRIB_UseStepList)
		return static_cast<uint32>(m_StepList.size());

	uint32 uiNumSteps = abs(m_iMax - m_iMin) / m_uiStep;
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
	if(m_iValue == iValue)
		return;

	m_iValue = iValue;

	FixValues();
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

// If iMax < iMin, iMin becomes the only legal value. An invalid 'uiStepAmt' will become 1
void HySlider::SetRange(int32 iMin, int32 iMax, uint32 uiStepAmt)
{
	if(iMax < iMin)
		m_iMin = m_iMax = iMin;
	else
	{
		m_iMin = iMin;
		m_iMax = iMax;
	}

	m_uiAttribs &= ~SLIDERATTRIB_UseStepList;
	m_uiStep = (uiStepAmt == 0 || uiStepAmt > static_cast<uint32>(m_iMax - m_iMin)) ? 1u : uiStepAmt;
	
	FixValues();
}

// An empty stepList is ignored
void HySlider::SetRange(const std::vector<int32> &stepList)
{
	if(stepList.empty())
		return;

	m_uiAttribs |= SLIDERATTRIB_UseStepList;
	m_StepList = stepList;
	std::sort(m_StepList.begin(), m_StepList.end());

	// Set m_iMin and m_iMax to values found in m_StepList
	m_iMin = m_iMax = m_StepList[0];
	for(uint32 i = 1; i < static_cast<uint32>(m_StepList.size()); ++i)
	{
		m_iMin = HyMin(m_iMin, m_StepList[i]);
		m_iMax = HyMax(m_iMax, m_StepList[i]);
	}
	
	FixValues();
}

HyOrientation HySlider::GetOrientation() const
{
	return (m_uiAttribs & SLIDERATTRIB_IsVertical) ? HYORIEN_Vertical : HYORIEN_Horizontal;
}

void HySlider::SetOrientation(HyOrientation eOrien)
{
	if(eOrien == HYORIEN_Horizontal)
		m_uiAttribs &= ~SLIDERATTRIB_IsVertical;
	else
		m_uiAttribs |= SLIDERATTRIB_IsVertical;

	Assemble();
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
	if(m_uiAttribs & SLIDERATTRIB_IsDragging)
	{
		if(HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft) == false)
		{
			m_uiAttribs &= ~SLIDERATTRIB_IsDragging;
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
		int32 iNumThresholds = static_cast<int32>(vDist[GetOrientation()] / fTickSpacing);
		int32 iNewValue = 0;
		if(iNumThresholds != 0)
		{
			if(m_uiAttribs & SLIDERATTRIB_UseStepList)
			{
				int32 iCurIndex = 0;
				for(int32 i = 0; i < static_cast<int32>(m_StepList.size()); ++i)
				{
					if(m_StepList[i] == m_iValue)
					{
						iCurIndex = i;
						break;
					}
				}
				iCurIndex = HyClamp(iCurIndex + iNumThresholds, 0, static_cast<int32>(m_StepList.size() - 1));
				iNewValue = m_StepList[iCurIndex];
			}
			else
				iNewValue = HyClamp(m_iValue + static_cast<int32>(iNumThresholds * m_uiStep), m_iMin, m_iMax);

			SetValue(iNewValue);
		}
	}
}

/*virtual*/ glm::vec2 HySlider::GetPosOffset() /*override*/
{
	if(GetOrientation() == HYORIEN_Horizontal)
		return glm::vec2(m_Slider.size.GetAnimFloat(HYORIEN_Horizontal).Get() * 0.5f, m_Slider.size.GetAnimFloat(HYORIEN_Vertical).Get() * 0.5f);
	else
		return glm::vec2(m_Slider.size.GetAnimFloat(HYORIEN_Horizontal).Get() * 0.5f, m_Slider.size.GetAnimFloat(HYORIEN_Vertical).Get() * 0.5f);
}

/*virtual*/ void HySlider::OnSetSizeHint() /*override*/
{
	float fRadius = GetBarRadius();

	if(GetOrientation() == HYORIEN_Horizontal)
		HySetVec(m_vSizeHint, static_cast<int32>(m_fLength + m_Slider.size.X()), static_cast<int32>(m_Slider.size.Y()));
	else
		HySetVec(m_vSizeHint, static_cast<int32>(m_Slider.size.X()), static_cast<int32>(m_fLength + m_Slider.size.Y()));
}

/*virtual*/ glm::ivec2 HySlider::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	m_Slider.size.GetAnimFloat(GetOrientation() ^ 1) = static_cast<float>(uiNewHeight);
	m_fLength = uiNewWidth - (m_Slider.size.GetAnimFloat(GetOrientation()).Get());
	OnSetSizeHint();

	Assemble();
	return m_vSizeHint;
}

/*virtual*/ void HySlider::OnUiMouseDown() /*override*/
{
	m_uiAttribs |= SLIDERATTRIB_IsDragging;
}

float HySlider::GetBarThickness()
{
	return m_Slider.size.GetAnimFloat(GetOrientation() ^ 1).Get() * 0.25f;
}

float HySlider::GetBarRadius()
{
	return GetBarThickness() * 0.5f;
}

void HySlider::Assemble()
{
	if(m_Slider.IsValid() == false)
		return;

	HyOrientation eOrientation = GetOrientation();

	if(eOrientation == HYORIEN_Horizontal)
		SetSizePolicy(HYSIZEPOLICY_Expanding, HYSIZEPOLICY_Fixed);
	else
		SetSizePolicy(HYSIZEPOLICY_Fixed, HYSIZEPOLICY_Expanding);

	// Build the slider bar out out of primitives
	float fBarThickness = GetBarThickness();
	m_Slider.pos.GetAnimFloat(eOrientation ^ 1) = m_Slider.size.GetAnimFloat(eOrientation ^ 1).Get() * -0.5f;
	
	m_BarStroke.Assemble(eOrientation, fBarThickness, m_fLength, 0.0f);
	m_BarFill.Assemble(eOrientation, fBarThickness, m_fLength, m_fStrokeAmt);

	// Now position the slider on the bar at the proper location based on current values
	FixValues();
}

void HySlider::FixValues()
{
	HyOrientation eOrientation = GetOrientation();

	if(m_uiAttribs & SLIDERATTRIB_UseStepList)
	{
		auto iter = std::find(m_StepList.begin(), m_StepList.end(), m_iValue);
		if(iter == m_StepList.end())
		{
			// TODO: Instead of testing against 'm_iValue', use the extrapolated value determined where the
			//       user is clicking on the slider bar. This fixes the value jumping back/forth when
			//       dragging the slider

			// Find closest value within m_StepList
			uint32 uiDiff = std::numeric_limits<uint32>::max();
			for(uint32 i = 0; i < m_StepList.size(); ++i)
			{
				if(uiDiff > static_cast<uint32>(abs(m_iValue - m_StepList[i])))
				{
					m_iValue = m_StepList[i];
					uiDiff = abs(m_iValue - m_StepList[i]);
				}
			}
		}
	}
	else
		m_iValue = HyClamp(m_iValue, m_iMin, m_iMax);

	float fLocalSliderPos = m_fLength * (static_cast<float>(m_iValue - m_iMin) / static_cast<float>(m_iMax - m_iMin));
	m_Slider.pos.GetAnimFloat(eOrientation) = fLocalSliderPos - (m_Slider.size.GetAnimFloat(eOrientation).Get() * 0.5f);

	auto ptCenter = m_Slider.GetSceneAABB().GetCenter();
	HySetVec(m_ptSliderCenter, ptCenter.x, ptCenter.y);

	m_BarStroke.m_BarPos.scale.GetAnimFloat(eOrientation) = fLocalSliderPos;
	m_BarStroke.m_BarNeg.pos.GetAnimFloat(eOrientation) = fLocalSliderPos;
	m_BarStroke.m_BarNeg.scale.GetAnimFloat(eOrientation) = m_fLength - fLocalSliderPos;

	m_BarFill.m_BarPos.scale.GetAnimFloat(eOrientation) = fLocalSliderPos;
	m_BarFill.m_BarNeg.pos.GetAnimFloat(eOrientation) = fLocalSliderPos;
	m_BarFill.m_BarNeg.scale.GetAnimFloat(eOrientation) = m_fLength - fLocalSliderPos;

	SetSizeAndLayoutDirty();
}
