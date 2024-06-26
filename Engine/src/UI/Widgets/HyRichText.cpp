/**************************************************************************
*	HyRichText.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyRichText.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Assets/Nodes/Objects/HyTextData.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyIO.h"

#include <regex>

HyRichText::HyRichText(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_pTextData(nullptr),
	m_vBoxDimensions(0.0f),
	m_eAlignment(HYALIGN_Left),
	m_fColumnLineHeightOffset(0.0f)
{
}

HyRichText::HyRichText(const HyPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_pTextData(nullptr),
	m_vBoxDimensions(0.0f),
	m_eAlignment(HYALIGN_Left),
	m_fColumnLineHeightOffset(0.0f)
{
	Setup(panelInit, HyNodePath(), HyMargins<float>());
}

HyRichText::HyRichText(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_pTextData(nullptr),
	m_vBoxDimensions(0.0f),
	m_eAlignment(HYALIGN_Left),
	m_fColumnLineHeightOffset(0.0f)
{
	Setup(panelInit, textNodePath, HyMargins<float>());
}

HyRichText::HyRichText(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_pTextData(nullptr),
	m_vBoxDimensions(0.0f),
	m_eAlignment(HYALIGN_Left),
	m_fColumnLineHeightOffset(0.0f)
{
	Setup(panelInit, textNodePath, textMargins);
}

/*virtual*/ HyRichText::~HyRichText()
{
	while(m_DrawableList.empty() == false)
	{
		delete m_DrawableList.back();
		m_DrawableList.pop_back();
	}
}

bool HyRichText::IsGlyphAvailable(std::string sUtf8Character) const
{
	if(m_pTextData == nullptr)
		return false;

	uint32 uiUsedBytes = 0;
	const HyTextGlyph *pGlyphRef = m_pTextData->GetGlyph(0, 0, HyIO::Utf8_to_Utf32(sUtf8Character.c_str(), uiUsedBytes));

	return pGlyphRef != nullptr;
}

/*virtual*/ float HyRichText::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	AssembleRichTextDrawables();
	return m_vBoxDimensions.x * fPercent;
}

/*virtual*/ float HyRichText::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	AssembleRichTextDrawables();
	return m_vBoxDimensions.y * fPercent;
}

float HyRichText::GetTextWidth(float fPercent /*= 1.0f*/)
{
	AssembleRichTextDrawables();

	float fTextWidth = 0.0f;
	ForEachDrawable([&fTextWidth](IHyDrawable2d *pDrawable) 
		{
			HyAssert(pDrawable->GetType() == HYTYPE_Text || pDrawable->GetType() == HYTYPE_Sprite, "HyRichText::GetTextWidth() - Drawable is not a text or sprite");
			if(pDrawable->GetType() == HYTYPE_Text)
				fTextWidth += (static_cast<HyText2d *>(pDrawable)->GetCursorPos().x - static_cast<HyText2d *>(pDrawable)->GetTextIndent());
			else // HYTYPE_Sprite
				fTextWidth += static_cast<HySprite2d *>(pDrawable)->GetStateWidth(pDrawable->GetState(), pDrawable->scale.GetX());
		});
	return fTextWidth * fPercent;
}

float HyRichText::GetTextHeight(float fPercent /*= 1.0f*/)
{
	AssembleRichTextDrawables();

	float fTextHeight = 0.0f;
	ForEachDrawable([&fTextHeight](IHyDrawable2d *pDrawable) { fTextHeight = HyMath::Max(fTextHeight, pDrawable->GetHeight(pDrawable->scale.GetY())); });
	return fTextHeight * fPercent;
}

void HyRichText::Setup(const HyPanelInit &panelInit)
{
	Setup(panelInit, m_TextPath, m_TextMargins);
}

void HyRichText::Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath)
{
	Setup(panelInit, textNodePath, m_TextMargins);
}

void HyRichText::Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins)
{
	SetSizePolicy(HYSIZEPOLICY_Fixed, HYSIZEPOLICY_Fixed);

	m_Panel.Setup(panelInit);
	m_TextPath = textNodePath;
	m_TextMargins = textMargins;

	HyText2d *pTempNewText = HY_NEW HyText2d(m_TextPath, this);
	m_pTextData = static_cast<const HyTextData *>(pTempNewText->AcquireData());
	if(m_pTextData == nullptr)
		HyLogWarning("HyRichText could not acquire data for text: " << pTempNewText->GetPrefix() << "/" << pTempNewText->GetName());
	delete pTempNewText;


	SetAsEnabled(IsEnabled());

	switch(GetTextType())
	{
	case HYTEXT_Line:		SetAsLine(); break;
	case HYTEXT_Column:		SetAsColumn(GetWidth()); break;
	case HYTEXT_Box:		SetAsBox(GetWidth(), GetHeight(), IsCenterVertically()); break;
	case HYTEXT_ScaleBox:	SetAsScaleBox(GetWidth(), GetHeight(), IsCenterVertically()); break;
	default:
		HyLogError("HyRichText::Setup() - Unhandled text type: " << GetTextType());
		break;
	}

	ResetDisplayOrder();

	MarkRichTextDirty();
	OnSetup();
}

HyTextType HyRichText::GetTextType() const
{
	return static_cast<HyTextType>((m_uiAttribs & RICHTEXTATTRIB_TextTypeMask) >> RICHTEXTATTRIB_TextTypeOffset);
}

void HyRichText::SetAsLine()
{
	HySetVec(m_vBoxDimensions, 0.0f, 0.0f);

	m_uiAttribs &= ~RICHTEXTATTRIB_IsCenterVertically;
	m_uiAttribs &= ~RICHTEXTATTRIB_TextTypeMask;
	m_uiAttribs |= HYTEXT_Line << RICHTEXTATTRIB_TextTypeOffset;
	MarkRichTextDirty();
}

void HyRichText::SetAsColumn(float fWidth)
{
	HySetVec(m_vBoxDimensions, fWidth, 0.0f);

	m_uiAttribs &= ~RICHTEXTATTRIB_IsCenterVertically;
	m_uiAttribs &= ~RICHTEXTATTRIB_TextTypeMask;
	m_uiAttribs |= HYTEXT_Column << RICHTEXTATTRIB_TextTypeOffset;
	MarkRichTextDirty();
}

void HyRichText::SetAsBox(float fWidth, float fHeight, bool bCenterVertically /*= false*/)
{
	HySetVec(m_vBoxDimensions, fWidth, fHeight);
	//SetScissor(HyRect(fWidth, fHeight));

	if(bCenterVertically)
		m_uiAttribs |= RICHTEXTATTRIB_IsCenterVertically;
	else
		m_uiAttribs &= ~RICHTEXTATTRIB_IsCenterVertically;

	m_uiAttribs &= ~RICHTEXTATTRIB_TextTypeMask;
	m_uiAttribs |= HYTEXT_Box << RICHTEXTATTRIB_TextTypeOffset;
	MarkRichTextDirty();
}

void HyRichText::SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically /*= true*/)
{
	HySetVec(m_vBoxDimensions, fWidth, fHeight);
	if(bCenterVertically)
		m_uiAttribs |= RICHTEXTATTRIB_IsCenterVertically;
	else
		m_uiAttribs &= ~RICHTEXTATTRIB_IsCenterVertically;

	m_uiAttribs &= ~RICHTEXTATTRIB_TextTypeMask;
	m_uiAttribs |= HYTEXT_ScaleBox << RICHTEXTATTRIB_TextTypeOffset;
	MarkRichTextDirty();
}

bool HyRichText::IsCenterVertically() const
{
	return (m_uiAttribs & RICHTEXTATTRIB_IsCenterVertically) != 0;
}

HyAlignment HyRichText::GetAlignment() const
{
	return m_eAlignment;
}

void HyRichText::SetAlignment(HyAlignment eAlignment)
{
	if(m_eAlignment == eAlignment)
		return;

	m_eAlignment = eAlignment;
	MarkRichTextDirty();
}

bool HyRichText::IsMonospacedDigits() const
{
	return (m_uiAttribs & RICHTEXTATTRIB_IsMonospacedDigits) != 0;
}

void HyRichText::SetMonospacedDigits(bool bEnable)
{
	if(IsMonospacedDigits() == bEnable)
		return;

	if(bEnable)
		m_uiAttribs |= RICHTEXTATTRIB_IsMonospacedDigits;
	else
		m_uiAttribs &= ~RICHTEXTATTRIB_IsMonospacedDigits;

	MarkRichTextDirty();
}

// Formatting examples:
// {1} = All text after this uses state 1
// {spritePrefix/spriteName,3} = Insert a sprite "spritePrefix/spriteName" with state 3, scaled to fit within text line
void HyRichText::SetText(const std::string &sRichTextFormat)
{
	m_sRichText = sRichTextFormat;
	MarkRichTextDirty();
}

void HyRichText::ForEachDrawable(std::function<void(IHyDrawable2d *)> fpForEachDrawable)
{
	AssembleRichTextDrawables();
	for(uint32 i = 0; i < m_DrawableList.size(); ++i)
		fpForEachDrawable(m_DrawableList[i]);
}

/*virtual*/ void HyRichText::Update() /*override*/
{
	IHyWidget::Update();
	AssembleRichTextDrawables();
}

/*virtual*/ glm::vec2 HyRichText::GetPosOffset() /*override*/
{
	AssembleRichTextDrawables();
	return glm::vec2(0.0f, m_vBoxDimensions.y - m_fColumnLineHeightOffset);
}

/*virtual*/ void HyRichText::OnSetSizeHint() /*override*/
{
	AssembleRichTextDrawables();
	HySetVec(m_vSizeHint, static_cast<int>(m_vBoxDimensions.x), static_cast<int>(m_vBoxDimensions.y));
}

/*virtual*/ glm::ivec2 HyRichText::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	switch(GetTextType())
	{
	case HYTEXT_Line:		SetAsLine(); break;
	case HYTEXT_Column:		SetAsColumn(static_cast<float>(uiNewWidth)); break;
	case HYTEXT_Box:		SetAsBox(static_cast<float>(uiNewWidth), static_cast<float>(uiNewHeight), IsCenterVertically()); break;
	case HYTEXT_ScaleBox:	SetAsScaleBox(static_cast<float>(uiNewWidth), static_cast<float>(uiNewHeight), IsCenterVertically()); break;
	default:
		HyLogError("HyRichText::Setup() - Unhandled text type: " << GetTextType());
		break;
	}

	AssembleRichTextDrawables();
	return glm::ivec2(GetWidth(), GetHeight());
}

void HyRichText::MarkRichTextDirty()
{
	m_uiAttribs |= RICHTEXTATTRIB_IsDirty;
}

void HyRichText::AssembleRichTextDrawables()
{
	if((m_uiAttribs & RICHTEXTATTRIB_IsDirty) == 0)
		return;

	std::vector<IHyDrawable2d *> newDrawableList;

	// Capture each formatting change within 'm_sRichText'
	std::regex rgx("\\{(.+?)\\}");
	std::smatch match;
	std::vector<std::pair<std::string, uint32>> formatChangeList;

	auto iterBegin = std::sregex_iterator(m_sRichText.begin(), m_sRichText.end(), rgx);
	auto iterEnd = std::sregex_iterator();
	for(std::sregex_iterator iter = iterBegin; iter != iterEnd; ++iter)
	{
		std::smatch match = *iter;
		std::string sMatch = match[1].str();

		std::smatch m;
		if(std::regex_search(sMatch, m, std::regex("([A-Za-z_]+[A-Za-z0-9_/]*,\\d+)"))) // Doesn't start with number and has ',' number param (sprite with state)
		{
			std::string sPath = m[1].str().substr(0, m[1].str().find(','));
			uint32 uiState = std::stoi(m[1].str().substr(m[1].str().find(',') + 1));

			formatChangeList.push_back(std::pair<std::string, uint32>(sPath, uiState));
		}
		else if(std::regex_search(sMatch, m, std::regex("([A-Za-z_]+[A-Za-z0-9_/]*)"))) // Doesn't start with number (either sprite or alignment)
		{
			std::string sPath = m[1].str();
			formatChangeList.push_back(std::pair<std::string, uint32>(sPath, 0));
		}
		else if(std::regex_search(sMatch, m, std::regex("(\\d+)"))) // Is only number (change text state)
		{
			formatChangeList.push_back(std::pair<std::string, uint32>("", std::stoi(m[1].str())));
		}
	}

	// Remove all formatting syntax from 'm_sRichText' and insert a delimiter in each place
	// Store results in a string stream 'ssCleanText' to process each delimited string
	std::istringstream ssCleanText(std::regex_replace(m_sRichText, rgx, "\x7F")); // "\x7F" is delete character (127) to be used as a delimiter to split each drawable

	// Reassemble the drawable list
	m_fColumnLineHeightOffset = 0.0f;

	std::string sCurText;
	uint32 uiCurTextState = 0;
	glm::vec2 ptCurPos(0.0f, 0.0f);
	float fUsedWidth = 0.0f;
	float fMaxLineDescender = 0.0f;
	bool bIsLoaded = true; // TODO: Store whether it's loaded here before creating any new drawables

	uint32 uiCurFmtIndex = 0;
	while(std::getline(ssCleanText, sCurText, '\x7F'))
	{
		float fLineDescender = 0.0f;
		float fLineHeight = 32.0f; // 32 is just some default value to fallback to
		if(m_pTextData)
		{
			fLineDescender = m_pTextData->GetLineDescender(uiCurTextState);
			fLineHeight = m_pTextData->GetLineHeight(uiCurTextState);
		}

		fMaxLineDescender = HyMath::Min(fMaxLineDescender, fLineDescender); // NOTE: Descenders are stored as negative values

		if(sCurText.empty() == false)
		{
			HyText2d *pNewText = HY_NEW HyText2d(m_TextPath, this);
			newDrawableList.push_back(pNewText);
			if(bIsLoaded)
				pNewText->Load();

			pNewText->SetTextIndent(static_cast<uint32>(ptCurPos.x));
			pNewText->SetAlignment(m_eAlignment);
			pNewText->SetMonospacedDigits(IsMonospacedDigits());
			pNewText->SetState(uiCurTextState);
			pNewText->SetText(sCurText);
			pNewText->pos.Set(0.0f, ptCurPos.y);

			switch(GetTextType())
			{
			case HYTEXT_Line:
				HySetVec(m_vBoxDimensions, 0.0f, 0.0f);
			case HYTEXT_ScaleBox: // ScaleBox is treated as a line, because it will be scaled to fit the box after all IHyDrawable's are created
				pNewText->SetAsLine();

				ptCurPos.x = pNewText->GetCursorPos().x;
				ptCurPos.y += pNewText->GetCursorPos().y;
				break;

			case HYTEXT_Column:
				m_vBoxDimensions.y = 0.0f;
			case HYTEXT_Box: // Box is treated as a column, because it will be cropped to fit the box after all IHyDrawable's are created
				pNewText->SetAsColumn(m_vBoxDimensions.x);

				ptCurPos.x = pNewText->GetCursorPos().x;
				ptCurPos.y += pNewText->GetCursorPos().y;
				break;

			default:
				HyLogError("HyRichText::AssembleDrawables() - Unhandled text type: " << GetTextType());
				break;
			}

			if(fUsedWidth < ptCurPos.x)
				fUsedWidth = ptCurPos.x;

			// TODO: 'm_fColumnLineHeightOffset' should actually be (I think) the max of first line's height, not the max line height overall
			const HyTextData *pTextData = static_cast<const HyTextData *>(pNewText->AcquireData());
			if(sCurText.empty() == false && pTextData)
				m_fColumnLineHeightOffset = HyMath::Max(m_fColumnLineHeightOffset, pTextData->GetLineHeight(uiCurTextState));
		}

		// Handle next format change
		if(uiCurFmtIndex < formatChangeList.size())
		{
			// If .first is empty string then just change text state
			if(formatChangeList[uiCurFmtIndex].first.empty())
				uiCurTextState = formatChangeList[uiCurFmtIndex].second;
			else // Otherwise insert sprite
			{
				//if(GetTextType() == HYTEXT_ScaleBox || GetTextType() == HYTEXT_Box)
				//	fLineHeight = m_vBoxDimensions.y;

				// Allocate sprite and initialize
				HySprite2d *pNewSprite = HY_NEW HySprite2d(HyNodePath(formatChangeList[uiCurFmtIndex].first.c_str()), this);
				newDrawableList.push_back(pNewSprite);
				if(bIsLoaded)
					pNewSprite->Load();

				pNewSprite->SetState(formatChangeList[uiCurFmtIndex].second);

				// Determine sprite scale with remaining room left on line
				float fScaleX = pNewSprite->GetStateWidth(pNewSprite->GetState());
				if(GetTextType() == HYTEXT_Box || GetTextType() == HYTEXT_Column) // NOTE: Don't check ScaleBox because that will be scaled to fit after assembling at full size
					fScaleX = (m_vBoxDimensions.x - ptCurPos.x) / pNewSprite->GetStateWidth(pNewSprite->GetState());
				float fScaleY = fLineHeight / pNewSprite->GetStateHeight(pNewSprite->GetState());
				
				pNewSprite->scale.SetAll(HyMath::Min(fScaleX, fScaleY));

				// If there's limited horizontal space, determine if this sprite will not fit in the remaining space on this text line
				if(GetTextType() == HYTEXT_Box || GetTextType() == HYTEXT_Column && // NOTE: Don't check ScaleBox because that will be scaled to fit after assembling at full size
					((ptCurPos.x + pNewSprite->GetStateWidth(pNewSprite->GetState(), pNewSprite->scale.X())) >= (m_vBoxDimensions.x - 1))) // the -1 should help with scale floating point above
				{
					fUsedWidth = m_vBoxDimensions.x;

					ptCurPos.x = 0.0f;
					ptCurPos.y -= fLineHeight;

					// Recalculate sprite scale with modified ptCurPos
					float fScaleX = (m_vBoxDimensions.x - ptCurPos.x) / pNewSprite->GetStateWidth(pNewSprite->GetState());
					float fScaleY = fLineHeight / pNewSprite->GetStateHeight(pNewSprite->GetState());
					pNewSprite->scale.SetAll(HyMath::Min(fScaleX, fScaleY));
				}
				
				pNewSprite->pos.Set(ptCurPos);

				// Find next drawable location and position 'ptCurPos' to it
				ptCurPos.x += pNewSprite->GetStateWidth(pNewSprite->GetState(), pNewSprite->scale.X());
				if(fUsedWidth < ptCurPos.x)
					fUsedWidth = ptCurPos.x;

				glm::vec2 vOffset = pNewSprite->GetStateOffset(pNewSprite->GetState());
				vOffset *= pNewSprite->scale.Get();
				pNewSprite->pos.Offset(-vOffset);

				if(m_vBoxDimensions.y == 0.0f)
				{
					// Also offset the sprite down by the descender amount, because at the moment they're sitting on the 'baseline'
					pNewSprite->pos.Offset(0.0f, -abs(fLineDescender));
				}
			}

			uiCurFmtIndex++;
		}
	}

	// Delete old drawable list, then replace with new list
	// NOTE: This is done at the end to avoid Harmony from potentially unloading and reloading the same assets (keeps ref count non-zero)
	while(m_DrawableList.empty() == false)
	{
		delete m_DrawableList.back();
		m_DrawableList.pop_back();
	}
	m_DrawableList = newDrawableList;

	float fUsedHeight = std::fabs(ptCurPos.y) + m_fColumnLineHeightOffset;

	if(GetTextType() == HYTEXT_ScaleBox)
	{
		// Scale all drawables to fit within the box
		float fScaleAmt = HyMath::Min(m_vBoxDimensions.x / fUsedWidth, m_vBoxDimensions.y / fUsedHeight);
		for(auto pDrawable : m_DrawableList)
		{
			if(pDrawable->GetType() == HYTYPE_Sprite)
			{
				pDrawable->pos.Set(pDrawable->pos.Get() * fScaleAmt);
				pDrawable->scale.Set(pDrawable->scale.GetX() * fScaleAmt, pDrawable->scale.GetY() * fScaleAmt);
			}
			else
				pDrawable->scale.SetAll(fScaleAmt);

			if(IsCenterVertically())
				pDrawable->pos.Offset(0.0f, (m_vBoxDimensions.y - m_fColumnLineHeightOffset) * 0.5f);
		}
	}

	// Ensure m_vBoxDimensions has the correct dimensions
	if(m_vBoxDimensions.x == 0.0f)
		m_vBoxDimensions.x = fUsedWidth;
	if(m_vBoxDimensions.y == 0.0f)
		m_vBoxDimensions.y = fUsedHeight;

	// Adjust the panel to fit around the text
	m_Panel.SetSize(m_vBoxDimensions.x, m_vBoxDimensions.y);
	switch(GetTextType())
	{
	case HYTEXT_Line:
		m_Panel.pos.Set(0.0f, fMaxLineDescender);
		break;

	case HYTEXT_ScaleBox:
	case HYTEXT_Column:
	case HYTEXT_Box:
		break;

	default:
		HyLogError("HyRichText::AssembleDrawables() - Unhandled text type: " << GetTextType());
		break;
	}

	// Inform everwhere that *this has been updated
	SetDirty(IHyNode::DIRTY_SceneAABB);
	SetSizeAndLayoutDirty();

	// But *this itself is no longer dirty
	m_uiAttribs &= ~RICHTEXTATTRIB_IsDirty;
}
