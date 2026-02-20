/**************************************************************************
*	HyLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Components/HyLayout.h"
#include "UI/HyUiContainer.h"
#include "UI/Widgets/IHyWidget.h"
#include "Diagnostics/Console/IHyConsole.h"

HyLayout::HyLayout(HyOrientation eLayoutType, int32 iWidgetSpacing, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_eLayoutType(eLayoutType),
	m_bReverse(m_eLayoutType == HYORIENT_Horizontal ? false : true), // 'm_bReverse' is defaulted ON when 'm_eLayoutType' is HYORIENT_Vertical to achieve top->bottom as default
	m_iWidgetSpacing(iWidgetSpacing)
{
	m_uiFlags |= NODETYPE_IsLayout;
}

/*virtual*/ HyLayout::~HyLayout()
{
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
		// Children are guaranteed to be IHyEntityUi
		IHyEntityUi *pItem = static_cast<IHyEntityUi *>(m_ChildList[i]);

		if(pItem->GetSizePolicy(eOrien) & HY_SIZEFLAG_EXPAND)
			uiSizePolicy |= HY_SIZEFLAG_EXPAND;
		if((pItem->GetSizePolicy(eOrien) & HY_SIZEFLAG_SHRINK) >> 1)
			uiSizePolicy |= HY_SIZEFLAG_SHRINK;
	}

	return static_cast<HySizePolicy>(uiSizePolicy);
}

/*virtual*/ glm::vec2 HyLayout::GetBotLeftOffset() /*override*/
{
	return glm::vec2(0.0f, 0.0f);
}

void HyLayout::AppendItem(IHyEntityUi &itemRef)
{
	ChildAppend(itemRef);
	SetSizeDirty();
}

bool HyLayout::RemoveItem(IHyEntityUi &itemRef)
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

bool HyLayout::IsReverseOrder()
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
		if((m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
			return static_cast<HyLayout *>(m_pParent)->RequestWidgetFocus(pWidget);
		else // m_pParent must be the container (and *this is the root layout)
			return static_cast<HyUiContainer *>(m_pParent)->RequestWidgetFocus(pWidget);
	}
	return false;
}

bool HyLayout::IsWidgetInputAllowed()
{
	// Propagate upward if this is nested in another layout
	if(m_pParent)
	{
		if((m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
			return static_cast<HyLayout *>(m_pParent)->IsWidgetInputAllowed();
		else // m_pParent must be the container (and *this is the root layout)
			return static_cast<HyUiContainer *>(m_pParent)->IsInputAllowed();
	}

	// This shouldn't happen
	return false;
}

#ifdef HY_USE_LAYOUT_DEBUG_BOXES
void HyLayout::ShowDebugBox(bool bShow)
{
	m_DebugBoxBoarder.alpha.Set(bShow ? 1.0f : 0.0f);
	m_DebugBoxMargins.alpha.Set(bShow ? 1.0f : 0.0f);
}

/*virtual*/ void HyLayout::OnUpdate() /*override*/
{
	const glm::mat4 &mtxSceneRef = GetSceneTransform(0.0f);
	glm::vec3 vScale(1.0f);
	glm::quat quatRot;
	glm::vec3 ptTranslation;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;
	glm::decompose(mtxSceneRef, vScale, quatRot, ptTranslation, vSkew, vPerspective);

	m_DebugBoxBoarder.pos.Set(ptTranslation);
	m_DebugBoxBoarder.rot.Set(glm::degrees(glm::atan(mtxSceneRef[0][1], mtxSceneRef[0][0])));
	m_DebugBoxBoarder.scale.Set(vScale);
	m_DebugBoxBoarder.UseWindowCoordinates(GetCoordinateSystem());
	m_DebugBoxBoarder.SetDisplayOrder(GetDisplayOrder() + 1);
	m_DebugBoxBoarder.SetVisible(IsVisible() && (GetInternalFlags() & EXPLICIT_ParentsVisible));

	if(m_Margins.left == 0 && m_Margins.right == 0 && m_Margins.top == 0 && m_Margins.bottom == 0)
		m_DebugBoxMargins.SetVisible(false);
	else
	{
		m_DebugBoxMargins.pos.Set(ptTranslation);
		m_DebugBoxMargins.pos.Offset(m_Margins.left, m_Margins.bottom);
		m_DebugBoxMargins.rot.Set(glm::degrees(glm::atan(mtxSceneRef[0][1], mtxSceneRef[0][0])));
		m_DebugBoxMargins.scale.Set(vScale);
		m_DebugBoxMargins.UseWindowCoordinates(GetCoordinateSystem());
		m_DebugBoxMargins.SetDisplayOrder(GetDisplayOrder() + 1);
		m_DebugBoxMargins.SetVisible(IsVisible() && (GetInternalFlags() & EXPLICIT_ParentsVisible));
	}
}

void HyLayout::OnSetDebugBox()
{
	m_DebugBoxBoarder.SetTint(HyColor::Blue);
	m_DebugBoxBoarder.SetWireframe(true);
	m_DebugBoxBoarder.SetAsBox(GetWidth(), GetHeight());

	m_DebugBoxMargins.SetTint(HyColor::Cyan);
	m_DebugBoxMargins.SetWireframe(true);
	m_DebugBoxMargins.SetAsBox(GetWidth() - m_Margins.left - m_Margins.right, GetHeight() - m_Margins.bottom - m_Margins.top);
}
#endif

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

		// Children are guaranteed to be IHyEntityUi
		IHyEntityUi *pItem = static_cast<IHyEntityUi *>(pChildItem);

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
		// Children are guaranteed to be IHyEntityUi
		IHyEntityUi *pItem = static_cast<IHyEntityUi *>(pChildItem);

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
		if(pItem->GetInternalFlags() & NODETYPE_IsLayout)
		{
			if(vTargetSize.x == 0)
				vResize.x = 0;
			if(vTargetSize.y == 0)
				vResize.y = 0;
		}
		glm::ivec2 vActualItemSize = pItem->Resize(vResize.x, vResize.y);
		if(vTargetSize[eOrientation] != 0 && vActualItemSize != vItemSize)
			bNeedsResize = true;

		// After resizing, apply offset to get 'pItem' oriented to its bottom left
		pItem->pos.Offset(pItem->GetBotLeftOffset());

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
		//SetSizeDirty();

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
	
#ifdef HY_USE_LAYOUT_DEBUG_BOXES
	OnSetDebugBox();
#endif

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
