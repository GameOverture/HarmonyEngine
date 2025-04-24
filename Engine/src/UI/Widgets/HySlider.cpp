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
void HySlider::BarPrimitives::DoAssembly(HyOrientation eOrientation, float fBarThickness, float fBarLength, float fIndentAmt)
{
	HyOrientation eInverseOrien = static_cast<HyOrientation>(eOrientation ^ 1);

	float fRadius = (fBarThickness * 0.5f) - fIndentAmt;
	m_EndCapNeg.pos.Set(0.0f, 0.0f);
	m_EndCapNeg.SetAsCircle(fRadius);

	m_EndCapPos.pos.GetAnimFloat(eOrientation) = fBarLength;
	m_EndCapPos.pos.GetAnimFloat(eInverseOrien) = 0.0f;
	m_EndCapPos.SetAsCircle(fRadius);

	if(eOrientation == HYORIENT_Horizontal)
	{
		m_BarPos.SetAsBox(HyRect(1.0f, fBarThickness - fIndentAmt));
		m_BarNeg.SetAsBox(HyRect(1.0f, fBarThickness - fIndentAmt));
	}
	else
	{
		m_BarPos.SetAsBox(HyRect(fBarThickness - fIndentAmt, 1.0f));
		m_BarNeg.SetAsBox(HyRect(fBarThickness - fIndentAmt, 1.0f));
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
	m_iStep(5),
	m_fLength(200.0f),
	m_fStrokeAmt(2.0f),
	m_iValue(0),
	m_BarStroke(this),
	m_BarFill(this),
	m_ptSliderCenter(0.0f, 0.0f),
	m_fpOnValueChanged(nullptr)
{
	RegisterAssembleEntity();
}

HySlider::HySlider(const HyUiPanelInit &sliderInitRef, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_iMin(0),
	m_iMax(100),
	m_iStep(5),
	m_fLength(200.0f),
	m_fStrokeAmt(2.0f),
	m_iValue(0),
	m_BarStroke(this),
	m_BarFill(this),
	m_ptSliderCenter(0.0f, 0.0f),
	m_fpOnValueChanged(nullptr)
{
	RegisterAssembleEntity();
	Setup(sliderInitRef);
}

/*virtual*/ HySlider::~HySlider()
{
}

/*virtual*/ float HySlider::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	OnSetSizeHint();
	Assemble();
	return m_vSizeHint.x * fPercent;
}

/*virtual*/ float HySlider::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	OnSetSizeHint();
	Assemble();
	return m_vSizeHint.y * fPercent;
}

void HySlider::Setup(const HyUiPanelInit &sliderInitRef)
{
	m_Panel.Setup(sliderInitRef);
	SetBarColors(m_Panel.GetTertiaryColor(), m_Panel.GetFrameColor(), m_Panel.GetFrameColor());
	ChildAppend(m_Panel); // Moves the panel to the front of the display order

	SetAsEnabled(IsEnabled());
	
	OnSetup();
	SetAssembleNeeded();
}

int64 HySlider::GetNumTicks() const
{
	if(m_uiAttribs & SLIDERATTRIB_UseStepList)
		return static_cast<uint32>(m_StepList.size());

	int64 iNumSteps = abs(m_iMax - m_iMin) / m_iStep;
	if(iNumSteps == 0)
		iNumSteps = 1;

	return iNumSteps;
}

int64 HySlider::GetValue() const
{
	return m_iValue;
}

void HySlider::SetValue(int64 iValue)
{
	if(m_iValue == iValue)
		return;

	m_iValue = iValue;

	SetAssembleNeeded();
	if(m_fpOnValueChanged)
		m_fpOnValueChanged(this);
}

int64 HySlider::GetMin() const
{
	return m_iMin;
}

int64 HySlider::GetMax() const
{
	return m_iMax;
}

// If iMax < iMin, iMin becomes the only legal value. An invalid 'iStepAmt' will become 1
void HySlider::SetRange(int64 iMin, int64 iMax, int32 iStepAmt)
{
	if(iMax < iMin)
		m_iMin = m_iMax = iMin;
	else
	{
		m_iMin = iMin;
		m_iMax = iMax;
	}

	m_uiAttribs &= ~SLIDERATTRIB_UseStepList;
	m_iStep = (iStepAmt <= 0 || iStepAmt > static_cast<int32>(m_iMax - m_iMin)) ? 1 : iStepAmt;
	
	SetAssembleNeeded();
}

// An empty stepList is ignored
void HySlider::SetRange(const std::vector<int64> &stepList)
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
		m_iMin = HyMath::Min(m_iMin, m_StepList[i]);
		m_iMax = HyMath::Max(m_iMax, m_StepList[i]);
	}
	
	SetAssembleNeeded();
}

HyOrientation HySlider::GetOrientation() const
{
	return (m_uiAttribs & SLIDERATTRIB_IsVertical) ? HYORIENT_Vertical : HYORIENT_Horizontal;
}

void HySlider::SetOrientation(HyOrientation eOrien)
{
	if(eOrien == HYORIENT_Horizontal)
		m_uiAttribs &= ~SLIDERATTRIB_IsVertical;
	else
		m_uiAttribs |= SLIDERATTRIB_IsVertical;

	SetAssembleNeeded();
}

void HySlider::SetBarColors(HyColor posColor, HyColor negColor, HyColor strokeColor)
{
	m_BarStroke.SetTint(strokeColor);

	m_BarFill.m_EndCapPos.SetTint(negColor, negColor.Darken());
	m_BarFill.m_BarPos.SetTint(posColor, posColor.Darken());
	m_BarFill.m_EndCapNeg.SetTint(posColor, posColor.Darken());
	m_BarFill.m_BarNeg.SetTint(negColor, negColor.Darken());
}

void HySlider::SetValueChangedCallback(std::function<void(HySlider *)> fpCallback)
{
	m_fpOnValueChanged = fpCallback;
}

/*virtual*/ void HySlider::Update() /*override*/
{
	IHyWidget::Update();

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
		int64 iNewValue = 0;
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
				iCurIndex = HyMath::Clamp(iCurIndex + iNumThresholds, 0, static_cast<int32>(m_StepList.size() - 1));
				iNewValue = m_StepList[iCurIndex];
			}
			else
				iNewValue = HyMath::Clamp(m_iValue + (iNumThresholds * m_iStep), m_iMin, m_iMax);

			SetValue(iNewValue);
		}
	}
}

/*virtual*/ void HySlider::OnAssemble() /*override*/
{
	if(m_Panel.IsAutoSize())
	{
		HyLogWarning("HySlider::OnAssemble() - Panel must have fixed size");
		return;
	}

	HyOrientation eOrientation = GetOrientation();

	if(eOrientation == HYORIENT_Horizontal)
		SetSizePolicy(HYSIZEPOLICY_Expanding, HYSIZEPOLICY_Fixed);
	else
		SetSizePolicy(HYSIZEPOLICY_Fixed, HYSIZEPOLICY_Expanding);

	// Build the slider bar out out of primitives
	float fBarThickness = GetBarThickness();
	m_Panel.pos.GetAnimFloat(eOrientation ^ 1) = m_Panel.GetSizeDimension(eOrientation ^ 1, -0.5f);

	m_BarStroke.DoAssembly(eOrientation, fBarThickness, m_fLength, 0.0f);
	m_BarFill.DoAssembly(eOrientation, fBarThickness, m_fLength, m_fStrokeAmt);

	// Now position the slider on the bar at the proper location based on current values
	if(m_uiAttribs & SLIDERATTRIB_UseStepList)
	{
		auto iter = std::find(m_StepList.begin(), m_StepList.end(), m_iValue);
		if(iter == m_StepList.end())
		{
			// TODO: Instead of testing against 'm_iValue', use the extrapolated value determined where the
			//       user is clicking on the slider bar. This fixes the value jumping back/forth when
			//       dragging the slider

			// Find closest value within m_StepList
			int64 iDiff = std::numeric_limits<int64>::max();
			for(uint32 i = 0; i < m_StepList.size(); ++i)
			{
				if(iDiff > static_cast<uint32>(abs(m_iValue - m_StepList[i])))
				{
					m_iValue = m_StepList[i];
					iDiff = abs(m_iValue - m_StepList[i]);
				}
			}
		}
	}
	else
		m_iValue = HyMath::Clamp(m_iValue, m_iMin, m_iMax);

	float fLocalSliderPos = m_fLength * (static_cast<float>(m_iValue - m_iMin) / static_cast<float>(m_iMax - m_iMin));
	m_Panel.pos.GetAnimFloat(eOrientation) = fLocalSliderPos - m_Panel.GetSizeDimension(eOrientation, 0.5f);

	auto ptCenter = b2AABB_Center(m_Panel.GetSceneAABB());
	HySetVec(m_ptSliderCenter, ptCenter.x, ptCenter.y);

	m_BarStroke.m_BarPos.scale.GetAnimFloat(eOrientation) = fLocalSliderPos;
	m_BarStroke.m_BarNeg.pos.GetAnimFloat(eOrientation) = fLocalSliderPos;
	m_BarStroke.m_BarNeg.scale.GetAnimFloat(eOrientation) = m_fLength - fLocalSliderPos;

	m_BarFill.m_BarPos.scale.GetAnimFloat(eOrientation) = fLocalSliderPos;
	m_BarFill.m_BarNeg.pos.GetAnimFloat(eOrientation) = fLocalSliderPos;
	m_BarFill.m_BarNeg.scale.GetAnimFloat(eOrientation) = m_fLength - fLocalSliderPos;

	//SetSizeAndLayoutDirty();
}

/*virtual*/ glm::vec2 HySlider::GetPosOffset() /*override*/
{
	return glm::vec2(m_Panel.GetWidth(m_Panel.scale.X()) * 0.5f, m_Panel.GetHeight(m_Panel.scale.Y()) * 0.5f);
}

/*virtual*/ void HySlider::OnSetSizeHint() /*override*/
{
	float fRadius = GetBarRadius();

	if(GetOrientation() == HYORIENT_Horizontal)
		HySetVec(m_vSizeHint, static_cast<int32>(m_fLength + m_Panel.GetWidth(m_Panel.scale.X())), static_cast<int32>(m_Panel.GetHeight(m_Panel.scale.Y())));
	else
		HySetVec(m_vSizeHint, static_cast<int32>(m_Panel.GetWidth(m_Panel.scale.X())), static_cast<int32>(m_fLength + m_Panel.GetHeight(m_Panel.scale.Y())));
}

/*virtual*/ glm::ivec2 HySlider::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	// TODO: Check if vertical breaks this... this seems incorrect

	m_Panel.SetSizeDimension(GetOrientation() ^ 1, uiNewHeight);

	m_fLength = uiNewWidth - m_Panel.GetSizeDimension(GetOrientation());
	OnSetSizeHint();

	SetAssembleNeeded();
	return m_vSizeHint;
}

/*virtual*/ void HySlider::OnUiMouseDown() /*override*/
{
	m_uiAttribs |= SLIDERATTRIB_IsDragging;
}

float HySlider::GetBarThickness()
{
	return m_Panel.GetSizeDimension(GetOrientation() ^ 1, 0.25f);
}

float HySlider::GetBarRadius()
{
	return GetBarThickness() * 0.5f;
}
