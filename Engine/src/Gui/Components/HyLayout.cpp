/**************************************************************************
*	HyLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	MIT License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/main/Engine/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Gui/Components/HyLayout.h"
#include "Gui/HyGui.h"
#include "Gui/Widgets/IHyWidget.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Window/HyWindow.h"
#include "Scene/Nodes/Objects/HyCamera.h"

HyLayout::HyLayout(HyOrientation eLayoutType, int32 iWidgetSpacing, HyEntity2d *pParent /*= nullptr*/) :
	IHyGuiBase(pParent),
	m_bIsRootLayout(false),
	m_eLayoutType(eLayoutType),
	m_bReverse(m_eLayoutType == HYORIENT_Horizontal ? false : true), // 'm_bReverse' is defaulted ON when 'm_eLayoutType' is HYORIENT_Vertical to achieve top->bottom as default
	m_iWidgetSpacing(iWidgetSpacing)
{
	m_uiFlags |= NODETYPE_IsGuiLayout;
}

/*virtual*/ HyLayout::~HyLayout()
{
}

bool HyLayout::IsRootLayout() const
{
	return m_bIsRootLayout;
}

void HyLayout::SetAsRootLayout(bool bIsRootLayout)
{
	m_bIsRootLayout = bIsRootLayout;
}

HyOrientation HyLayout::GetLayoutType() const
{
	return m_eLayoutType;
}

void HyLayout::SetLayoutType(HyOrientation eLayoutType)
{
	if(m_eLayoutType == eLayoutType)
		return;

	m_eLayoutType = eLayoutType;
	m_bReverse = (m_eLayoutType == HYORIENT_Horizontal ? false : true), // 'm_bReverse' is defaulted ON when 'm_eLayoutType' is HYORIENT_Vertical to achieve top->bottom as default
	SetSizeDirty();
}

/*virtual*/ HySizePolicy HyLayout::GetSizePolicy(HyOrientation eOrien) const /*override*/
{
	uint32 uiSizePolicy = 0;// HYSIZEPOLICY_Fixed;
	bool bShrinkValid = true;
	for(int32 i = 0; i < static_cast<int32>(m_ChildList.size()); ++i)
	{
		HyAssert(m_ChildList[i]->GetInternalFlags() & NODETYPE_IsGui, "HyLayout::GetSizePolicy() - m_ChildList contains a non-Gui item");

		// Children are guaranteed to be IHyGuiBase
		IHyGuiBase *pItem = static_cast<IHyGuiBase *>(m_ChildList[i]);

		if(pItem->GetSizePolicy(eOrien) & HY_SIZEFLAG_EXPAND)
			uiSizePolicy |= HY_SIZEFLAG_EXPAND;
		if((pItem->GetSizePolicy(eOrien) & HY_SIZEFLAG_SHRINK) >> 1)
			uiSizePolicy |= HY_SIZEFLAG_SHRINK;
	}

	return static_cast<HySizePolicy>(uiSizePolicy);
}

void HyLayout::AppendItem(IHyGuiBase &itemRef)
{
	ChildAppend(itemRef);
	SetSizeDirty();
}

bool HyLayout::RemoveItem(IHyGuiBase &itemRef)
{
	if(ChildRemove(&itemRef))
	{
		SetSizeDirty();
		return true;
	}

	return false;
}

void HyLayout::DetachAllItems()
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();

	SetSizeDirty();
}

bool HyLayout::IsReverseOrder() const
{
	if(m_eLayoutType == HYORIENT_Vertical)
		return !m_bReverse; // HYORIENT_Vertical needs this flipped to achieve top->bottom default
	else
		return m_bReverse;
}

void HyLayout::ReverseOrder(bool bReverse)
{
	if(m_eLayoutType == HYORIENT_Vertical)
		m_bReverse = !bReverse; // HYORIENT_Vertical needs this flipped to achieve top->bottom default
	else
		m_bReverse = bReverse;

	SetSizeDirty();
}

const HyMargins<int16> &HyLayout::GetMargins() const
{
	return m_Margins;
}

void HyLayout::SetMargins(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, int32 iWidgetSpacing)
{
	m_Margins.Set(iLeft, iBottom, iRight, iTop);
	m_iWidgetSpacing = iWidgetSpacing;
	SetSizeDirty();
}

void HyLayout::SetMargins(const HyMargins<int16> &newMargins, int32 iWidgetSpacing)
{
	m_Margins = newMargins;
	m_iWidgetSpacing = iWidgetSpacing;
	SetSizeDirty();
}

int32 HyLayout::GetWidgetSpacing()
{
	return m_iWidgetSpacing;
}

bool HyLayout::RequestWidgetFocus(IHyWidget *pWidget)
{
	// Propagate upward if this is nested in another layout
	if(m_pParent)
	{
		if((m_pParent->GetInternalFlags() & NODETYPE_IsGuiLayout) != 0)
			return static_cast<HyLayout *>(m_pParent)->RequestWidgetFocus(pWidget);
		else // m_pParent must be the container (and *this is the root layout)
			return static_cast<HyGui *>(m_pParent)->RequestWidgetFocus(pWidget);
	}
	return false;
}

bool HyLayout::IsWidgetInputAllowed()
{
	// Propagate upward if this is nested in another layout
	if(m_pParent)
	{
		if((m_pParent->GetInternalFlags() & NODETYPE_IsGuiLayout) != 0)
			return static_cast<HyLayout *>(m_pParent)->IsWidgetInputAllowed();
		else // m_pParent must be the container (and *this is the root layout)
			return static_cast<HyGui *>(m_pParent)->IsInputAllowed();
	}

	// This shouldn't happen
	return false;
}

void HyLayout::SetDebugBox(HyPrimitive2d &debugBox, HyCamera2d *pCamera)
{
	if(GetWindowIndex() >= 0 && GetWindowIndex() != pCamera->GetWindow().GetIndex())
	{
		HyLogWarning("HyLayout::SetDebugBox was passed a camera that is incompatible with this layout");
		return;
	}

	const glm::mat4 &mtxSceneRef = GetSceneTransform(0.0f);
	glm::vec3 ptTranslation = mtxSceneRef[3];
	glm::vec3 vScale = glm::vec3(glm::length(mtxSceneRef[0]), glm::length(mtxSceneRef[1]), glm::length(mtxSceneRef[2]));
	double dRotation = glm::atan(mtxSceneRef[0][1], mtxSceneRef[0][0]);
	float fWidth = GetWidth(vScale.x);
	float fHeight = GetHeight(vScale.y);

	glm::vec2 pt1, pt2, pt3, pt4; // Counter-clockwise winding order of box dimensions
	pt1 = { ptTranslation.x, ptTranslation.y };
	pt2 = { ptTranslation.x + (fWidth * glm::cos(dRotation)), ptTranslation.y + (fWidth * glm::sin(dRotation)) };
	pt3 = { ptTranslation.x + (fWidth * glm::cos(dRotation)) - (fHeight * glm::sin(dRotation)), ptTranslation.y + (fWidth * glm::sin(dRotation)) + (fHeight * glm::cos(dRotation)) };
	pt4 = { ptTranslation.x - (fHeight * glm::sin(dRotation)), ptTranslation.y + (fHeight * glm::cos(dRotation)) };

	glm::vec2 vDir1 = glm::normalize(pt2 - pt1); // left-right
	glm::vec2 vDir2 = glm::normalize(pt4 - pt1); // up-down
	glm::vec2 ptMargin1, ptMargin2, ptMargin3, ptMargin4;
	ptMargin1 = { pt1.x + (m_Margins.left * vDir1.x) + (m_Margins.bottom * vDir2.x), pt1.y + (m_Margins.left * vDir1.y) + (m_Margins.bottom * vDir2.y) };
	ptMargin2 = { pt2.x - (m_Margins.right * vDir1.x) + (m_Margins.bottom * vDir2.x), pt2.y - (m_Margins.right * vDir1.y) + (m_Margins.bottom * vDir2.y) };
	ptMargin3 = { pt3.x - (m_Margins.right * vDir1.x) - (m_Margins.top * vDir2.x), pt3.y - (m_Margins.right * vDir1.y) - (m_Margins.top * vDir2.y) };
	ptMargin4 = { pt4.x + (m_Margins.left * vDir1.x) - (m_Margins.top * vDir2.x), pt4.y + (m_Margins.left * vDir1.y) - (m_Margins.top * vDir2.y) };

	if(GetWindowIndex() < 0) // Worldspace
	{
		pCamera->ProjectToCamera(pt1, pt1);
		pCamera->ProjectToCamera(pt2, pt2);
		pCamera->ProjectToCamera(pt3, pt3);
		pCamera->ProjectToCamera(pt4, pt4);

		pCamera->ProjectToCamera(ptMargin1, ptMargin1);
		pCamera->ProjectToCamera(ptMargin2, ptMargin2);
		pCamera->ProjectToCamera(ptMargin3, ptMargin3);
		pCamera->ProjectToCamera(ptMargin4, ptMargin4);
	}

	debugBox.UseWindowCoordinates(pCamera->GetWindow().GetIndex());
	debugBox.SetDisplayOrder(GetDisplayOrder() + 1);
	debugBox.SetVisible(IsVisible() && (GetInternalFlags() & EXPLICIT_ParentsVisible));

	debugBox.SetAsBox(LAYER_Border, HyRect(pt1, pt2, pt3, pt4), 1.0f);
	debugBox.SetLayerColor(LAYER_Border, HyColor::Green);

	debugBox.SetAsBox(LAYER_Margin, HyRect(ptMargin1, ptMargin2, ptMargin3, ptMargin4), 1.0f);
	debugBox.SetLayerColor(LAYER_Margin, HyColor::DarkGreen);
	if(m_Margins.left == 0 && m_Margins.right == 0 && m_Margins.top == 0 && m_Margins.bottom == 0)
		debugBox.SetLayerVisible(LAYER_Margin, false);
	else
		debugBox.SetLayerVisible(LAYER_Margin, true);
}

/*virtual*/ glm::ivec2 HyLayout::OnCalcPreferredSize() /*override*/
{
	glm::ivec2 vSizeHint(m_Margins.left + m_Margins.right, m_Margins.bottom + m_Margins.top);
	glm::ivec2 vMinSize(vSizeHint);

	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return vMinSize;

	HyOrientation eOrientation, eInverseOrien;
	if(m_eLayoutType == HYORIENT_Horizontal)
	{
		eOrientation = HYORIENT_Horizontal;
		eInverseOrien = HYORIENT_Vertical;
	}
	else
	{
		eOrientation = HYORIENT_Vertical;
		eInverseOrien = HYORIENT_Horizontal;
	}

	// Figure out vSizeHint while counting size policies
	vSizeHint[eOrientation] += (GetWidgetSpacing() * (uiNumChildren - 1));
	vMinSize[eOrientation] += (GetWidgetSpacing() * (uiNumChildren - 1));
	m_uiNumExpandItems = m_uiNumShrinkItems = 0;

	for(uint32 i = 0; i < static_cast<uint32>(m_ChildList.size()); ++i)
	{
		IHyNode2d *pChildItem = m_ChildList[i];
		HyAssert(pChildItem->GetInternalFlags() & NODETYPE_IsGui, "HyLayout::OnCalcPreferredSize() - m_ChildList contains a non-Gui item");

		// Children are guaranteed to be IHyGuiBase
		IHyGuiBase *pItem = static_cast<IHyGuiBase *>(pChildItem);

		glm::ivec2 vItemMinSize = pItem->GetMinSize();
		vMinSize[eOrientation] += vItemMinSize[eOrientation];
		vMinSize[eInverseOrien] = HyMath::Max(vMinSize[eInverseOrien], vItemMinSize[eInverseOrien]);

		m_uiNumExpandItems += (pItem->GetSizePolicy(eOrientation) & HY_SIZEFLAG_EXPAND);		// Adds 1 or 0
		m_uiNumShrinkItems += ((pItem->GetSizePolicy(eOrientation) & HY_SIZEFLAG_SHRINK) >> 1);	// Adds 1 or 0

		glm::ivec2 vItemSizeHint = pItem->GetPreferredSize();
		vSizeHint[eOrientation] += vItemSizeHint[eOrientation];
		vSizeHint[eInverseOrien] = HyMath::Max(vSizeHint[eInverseOrien], vItemSizeHint[eInverseOrien]);
	}

	SetMinSize(vMinSize.x, vMinSize.y);
	return vSizeHint;
}

/*virtual*/ glm::ivec2 HyLayout::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	glm::ivec2 vTargetSize(uiNewWidth, uiNewHeight);
	if(m_vActualSize == vTargetSize && IsSizeDirty() == false)
		return m_vActualSize;

	HyOrientation eOrientation, eInverseOrien;
	int32 iInverseOrienMargin;
	if(m_eLayoutType == HYORIENT_Horizontal)
	{
		eOrientation = HYORIENT_Horizontal;
		eInverseOrien = HYORIENT_Vertical;
		iInverseOrienMargin = m_Margins.top + m_Margins.bottom;
	}
	else
	{
		eOrientation = HYORIENT_Vertical;
		eInverseOrien = HYORIENT_Horizontal;
		iInverseOrienMargin = m_Margins.left + m_Margins.right;
	}

	glm::ivec2 vSizeHint = GetPreferredSize();
	float fExpandAmt = 0.0f, fShrinkAmt = 0.0f;
	if(vTargetSize[eOrientation] != 0)
		GetDistributedScalingAmts(vTargetSize[eOrientation], vSizeHint[eOrientation], fExpandAmt, fShrinkAmt);

	// Go through each child and set its position and OnResize()
	glm::vec2 ptCurPos(m_Margins.left, m_Margins.bottom);
	int32 iMaxInverse = 0; // Used to determine the m_vActualSize when the inverse dimension was specified as '0'
	bool bNeedsResize = false;

	auto fpPositionAndResize = [&](IHyNode2d *&pChildItem) // Lambda func used to iterate over 'm_ChildList'
	{
		HyAssert(pChildItem->GetInternalFlags() & NODETYPE_IsGui, "HyLayout::OnResize() - fpPositionAndResize was passed a non-Gui item");

		// Children are guaranteed to be IHyGuiBase
		IHyGuiBase *pItem = static_cast<IHyGuiBase *>(pChildItem);

		// Set item to the ptCurPos
		pItem->pos.Set(ptCurPos);

		glm::ivec2 vItemSize = pItem->GetPreferredSize();

		if(vTargetSize[eOrientation] != 0)
		{
			HySizePolicy eSizePolicy = pItem->GetSizePolicy(eOrientation);
			// Along 'eOrientation', distribute either 'fExpandAmt' or 'fShrinkAmt' among those with those size policies
			vItemSize[eOrientation] += static_cast<int32>(fExpandAmt * (eSizePolicy & HY_SIZEFLAG_EXPAND));
			vItemSize[eOrientation] += static_cast<int32>(fShrinkAmt * ((eSizePolicy & HY_SIZEFLAG_SHRINK) >> 1));
		}

		if(vTargetSize[eInverseOrien] != 0)
		{
			// Along 'eInverseOrien', either clamp size or center within available space
			HySizePolicy eSizePolicy = pItem->GetSizePolicy(eInverseOrien);
			if(vItemSize[eInverseOrien] <= vTargetSize[eInverseOrien])
			{
				if((eSizePolicy & HY_SIZEFLAG_EXPAND) != 0)
					vItemSize[eInverseOrien] += (vTargetSize[eInverseOrien] - iInverseOrienMargin) - vItemSize[eInverseOrien];
				else
					pItem->pos.GetAnimFloat(eInverseOrien).Offset(0.5f * ((vTargetSize[eInverseOrien] - iInverseOrienMargin) - vItemSize[eInverseOrien]));
			}
			else
			{
				if((eSizePolicy & HY_SIZEFLAG_SHRINK) != 0)
					vItemSize[eInverseOrien] += (vTargetSize[eInverseOrien] - iInverseOrienMargin) - vItemSize[eInverseOrien];
			}
		}
		else
			iMaxInverse = HyMath::Max(iMaxInverse, vItemSize[eInverseOrien]);

		// If 'pItem' is a nested layout, it should pass '0' to Resize where appropriate
		glm::ivec2 vResize = vItemSize;
		if(pItem->GetInternalFlags() & NODETYPE_IsGuiLayout)
		{
			if(vTargetSize.x == 0)
				vResize.x = 0;
			if(vTargetSize.y == 0)
				vResize.y = 0;
		}
		glm::ivec2 vActualItemSize = pItem->Resize(vResize.x, vResize.y);
		if(vTargetSize[eOrientation] != 0 && vActualItemSize != vItemSize)
			bNeedsResize = true;

		if(0 == (pItem->GetInternalFlags() & NODETYPE_IsGuiLayout))
			pItem->pos.Offset(pItem->GetWidth(0.5f), pItem->GetHeight(0.5f));

		ptCurPos[eOrientation] += vActualItemSize[eOrientation] + GetWidgetSpacing();
	};
	if(m_bReverse)
		std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPositionAndResize);
	else
		std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPositionAndResize);

	if(vTargetSize[eInverseOrien] == 0)
	{
		vTargetSize[eInverseOrien] = iMaxInverse;
		bNeedsResize = true;
	}

	if(bNeedsResize) // TODO: Investigate this
	{
		vTargetSize[eOrientation] = static_cast<int32>(ptCurPos[eOrientation] - GetWidgetSpacing());
		HySetVec(ptCurPos, m_Margins.left, m_Margins.bottom);

		GetDistributedScalingAmts(vTargetSize[eOrientation], GetPreferredSize()[eOrientation], fExpandAmt, fShrinkAmt);

		if(m_bReverse)
			std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPositionAndResize);
		else
			std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPositionAndResize);
	}

	m_vActualSize[eOrientation] = static_cast<int32>(ptCurPos[eOrientation] - GetWidgetSpacing());
	m_vActualSize[eInverseOrien] = vTargetSize[eInverseOrien];

	if(vTargetSize[eOrientation] > m_vActualSize[eOrientation])
		m_vActualSize[eOrientation] = vTargetSize[eOrientation];
	if(vTargetSize[eInverseOrien] > m_vActualSize[eInverseOrien])
		m_vActualSize[eInverseOrien] = vTargetSize[eInverseOrien];

	return m_vActualSize;
}

void HyLayout::GetDistributedScalingAmts(int32 iTargetLength, int32 iCurrLength, float &fExpandAmtOut, float &fShrinkAmtOut)
{
	// Determine if current length fits within iTargetLength
	float fDifference = static_cast<float>(iTargetLength) - static_cast<float>(iCurrLength);
	
	fExpandAmtOut = 0.0f;
	fShrinkAmtOut = 0.0f;

	// Distribute positive difference to all 'expanding' size policies
	if(fDifference >= 0)
	{
		if(m_uiNumExpandItems > 0)
			fExpandAmtOut = fDifference / m_uiNumExpandItems;
	}
	else // Distribute negative difference to all 'shrink' size policies
	{
		if(m_uiNumShrinkItems > 0)
			fShrinkAmtOut = fDifference / m_uiNumShrinkItems;
	}
}
