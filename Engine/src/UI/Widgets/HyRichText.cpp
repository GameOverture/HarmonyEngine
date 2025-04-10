/**************************************************************************
*	HyRichLabel.cpp
*
*	Harmony Engine
*	Copyright (c) 2024 Jason Knobler
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

HyRichLabel::HyRichLabel(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_vTextDimensions(0.0f)
{
}

HyRichLabel::HyRichLabel(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(panelInit, textInit, pParent),
	m_vTextDimensions(0.0f)
{
}

/*virtual*/ HyRichLabel::~HyRichLabel()
{
	while(m_DrawableList.empty() == false)
	{
		delete m_DrawableList.back();
		m_DrawableList.pop_back();
	}
}

/*virtual*/ float HyRichLabel::GetTextWidth(float fPercent /*= 1.0f*/) /*override*/
{
	Assemble();
	return m_vTextDimensions.x * fPercent;
}

/*virtual*/ float HyRichLabel::GetTextHeight(float fPercent /*= 1.0f*/) /*override*/
{
	Assemble();
	return m_vTextDimensions.y * fPercent;
}

/*virtual*/ std::string HyRichLabel::GetUtf8String() const /*override*/
{
	return m_sRichText;
}

/*virtual*/ void HyRichLabel::SetText(const std::string &sUtf8RichText) /*override*/
{
	m_sRichText = sUtf8RichText;
	SetAssembleNeeded();
}

void HyRichLabel::ForEachDrawable(std::function<void(IHyDrawable2d *)> fpForEachDrawable)
{
	Assemble();
	for(uint32 i = 0; i < m_DrawableList.size(); ++i)
		fpForEachDrawable(m_DrawableList[i]);
}

/*virtual*/ void HyRichLabel::OnAssemble() /*override*/
{
	const HyTextData *pTextData = static_cast<const HyTextData *>(m_Text.AcquireData());
	if(pTextData == nullptr || GetTextNodePath().IsValid() == false)
		return;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Store whether the current m_DrawableList are supposed to be loaded
	bool bIsDrawablesLoaded = true;
	for(uint32 i = 0; i < m_DrawableList.size(); ++i)
	{
		if(m_DrawableList[i]->GetLoadState() != HYLOADSTATE_Queued && m_DrawableList[i]->GetLoadState() != HYLOADSTATE_Loaded)
		{
			bIsDrawablesLoaded = false;
			break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Reassemble the drawable list using 'ssCleanText' and 'formatChangeList'
	std::vector<IHyDrawable2d *> newDrawableList;

	float fLargestLineAscender = 0.0f;
	float fLargestLineDescender = 0.0f;
	float fLineHeight = 0.0f;
	const float fAVAILABLE_TEXT_WIDTH = m_Panel.GetWidth() - m_TextMargins.left - m_TextMargins.right;

	glm::vec2 ptCursorPos(0.0f, 0.0f);
	uint32 uiCurFmtIndex = 0;
	uint32 uiCurTextState = 0;
	std::string sCurText;
	while(std::getline(ssCleanText, sCurText, '\x7F'))
	{
		if(sCurText.empty() == false)
		{
			HyText2d *pNewText = HY_NEW HyText2d(GetTextNodePath(), this);
			newDrawableList.push_back(pNewText);
			if(bIsDrawablesLoaded)
				pNewText->Load();

			pNewText->SetTextIndent(static_cast<uint32>(ptCursorPos.x));
			pNewText->SetMonospacedDigits(IsMonospacedDigits());
			pNewText->SetState(uiCurTextState);
			pNewText->SetText(sCurText);
			pNewText->pos.Set(0.0f, ptCursorPos.y);

			for(uint32 uiCharIndex = 0; uiCharIndex < pNewText->GetNumCharacters(); ++uiCharIndex)
			{
				for(uint32 uiLayerIndex = 0; uiLayerIndex < pNewText->GetNumLayers(uiCurTextState); ++uiLayerIndex)
				{
					const HyTextGlyph *pGlyph = pTextData->GetGlyph(uiCurTextState, uiLayerIndex, pNewText->GetCharacterCode(uiCharIndex));
					if(pGlyph)
					{
						fLargestLineAscender = HyMath::Max(fLargestLineAscender, static_cast<float>(pGlyph->iOFFSET_Y));
						fLargestLineDescender = HyMath::Min(fLargestLineDescender, static_cast<float>(pGlyph->iOFFSET_Y) - static_cast<float>(pGlyph->uiHEIGHT)); // NOTE: Descenders are stored as negative values
					}
				}
			}
			
			switch(GetTextType())
			{
			case HYTEXT_Line:
				[[fallthrough]];
			case HYTEXT_ScaleBox: // ScaleBox is treated as a line, because it will be scaled to fit the box after all IHyDrawable's are created
				pNewText->SetAsLine();

				fLineHeight = HyMath::Max(fLineHeight, pNewText->GetHeight());

				ptCursorPos.x = pNewText->GetCursorPos().x;
				ptCursorPos.y += pNewText->GetCursorPos().y;
				break;

			case HYTEXT_Column:
				[[fallthrough]];
			case HYTEXT_Box: // Box is treated as a column, because it will be cropped to fit the box after all IHyDrawable's are created
				pNewText->SetAsColumn(fAVAILABLE_TEXT_WIDTH);

				fLineHeight = HyMath::Max(fLineHeight, pTextData->GetLineHeight(uiCurTextState));

				ptCursorPos.x = pNewText->GetCursorPos().x;
				ptCursorPos.y += pNewText->GetCursorPos().y;
				break;

			default:
				HyLogError("HyRichText::AssembleDrawables() - Unhandled text type: " << GetTextType());
				break;
			}
		}

		// Handle next format change
		if(uiCurFmtIndex < formatChangeList.size())
		{
			// If .first is empty string then just change text state
			if(formatChangeList[uiCurFmtIndex].first.empty())
				uiCurTextState = formatChangeList[uiCurFmtIndex].second;
			else // Otherwise insert sprite
			{
				// Allocate sprite and initialize
				HySprite2d *pNewSprite = HY_NEW HySprite2d(HyNodePath(formatChangeList[uiCurFmtIndex].first.c_str()), this);
				newDrawableList.push_back(pNewSprite);
				if(bIsDrawablesLoaded)
					pNewSprite->Load();

				pNewSprite->SetState(formatChangeList[uiCurFmtIndex].second);

				// Determine sprite scale with remaining room left on line
				float fScaleX = pNewSprite->GetStateWidth(pNewSprite->GetState());
				if(GetTextType() == HYTEXT_Box || GetTextType() == HYTEXT_Column) // NOTE: Don't check ScaleBox because that will be scaled to fit after assembling at full size
					fScaleX = (fAVAILABLE_TEXT_WIDTH - ptCursorPos.x) / pNewSprite->GetStateWidth(pNewSprite->GetState());

				float fScaleY = fLineHeight / pNewSprite->GetStateHeight(pNewSprite->GetState());

				pNewSprite->scale.SetAll(HyMath::Min(fScaleX, fScaleY));

				// If there's limited horizontal space, determine if this sprite will not fit in the remaining space on this text line
				if(GetTextType() == HYTEXT_Box || GetTextType() == HYTEXT_Column && // NOTE: Don't check ScaleBox because that will be scaled to fit after assembling at full size
					((ptCursorPos.x + pNewSprite->GetStateWidth(pNewSprite->GetState(), pNewSprite->scale.X())) >= (fAVAILABLE_TEXT_WIDTH - 1))) // the -1 should help with scale floating point above
				{
					ptCursorPos.x = 0.0f;
					ptCursorPos.y -= fLineHeight;

					// Recalculate sprite scale with modified ptCursorPos
					float fScaleX = (fAVAILABLE_TEXT_WIDTH - ptCursorPos.x) / pNewSprite->GetStateWidth(pNewSprite->GetState());
					float fScaleY = fLineHeight / pNewSprite->GetStateHeight(pNewSprite->GetState());
					pNewSprite->scale.SetAll(HyMath::Min(fScaleX, fScaleY));
				}

				pNewSprite->pos.Set(ptCursorPos);

				// Find next drawable location and position 'ptCursorPos' to it
				ptCursorPos.x += pNewSprite->GetStateWidth(pNewSprite->GetState(), pNewSprite->scale.X());

				glm::vec2 vOffset = pNewSprite->GetStateOffset(pNewSprite->GetState());
				vOffset *= pNewSprite->scale.Get();
				pNewSprite->pos.Offset(-vOffset);

				// Also offset the sprite down by the descender amount, because at the moment they're sitting on the 'baseline'
				pNewSprite->pos.Offset(0.0f, fLargestLineDescender);
			}

			uiCurFmtIndex++;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Determine final dimensions of rich text using the assembled drawables
	HySetVec(m_vTextDimensions, 0.0f, 0.0f);
	switch(GetTextType())
	{
	case HYTEXT_Line:
	case HYTEXT_ScaleBox:
		m_vTextDimensions.x = ptCursorPos.x;
		m_vTextDimensions.y = fLineHeight;
		break;

	case HYTEXT_Vertical:
	default:
		HyLogError("HyRichText::AssembleDrawables() - Unhandled text type: " << GetTextType());
		break;

	case HYTEXT_Box:
	case HYTEXT_Column:
		m_vTextDimensions.x = fAVAILABLE_TEXT_WIDTH;
		m_vTextDimensions.y = std::fabs(ptCursorPos.y + fLargestLineAscender);
		break;
	}

	if(m_Panel.IsAutoSize())
		m_Panel.SetSize(m_vTextDimensions.x + m_TextMargins.left + m_TextMargins.right, m_vTextDimensions.y + m_TextMargins.top + m_TextMargins.bottom);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Delete old drawable list, then replace with new list
	// NOTE: This is done at the end to avoid Harmony from potentially unloading and reloading the same assets (keeps ref count non-zero)
	while(m_DrawableList.empty() == false)
	{
		delete m_DrawableList.back();
		m_DrawableList.pop_back();
	}
	m_DrawableList = newDrawableList;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Based on text type and alignment, adjust the items in 'm_DrawableList' accordingly
	switch(GetTextType())
	{
	case HYTEXT_Line:
	case HYTEXT_Column:
	case HYTEXT_Box:
		for(auto pDrawable : m_DrawableList)
			pDrawable->pos.Offset(m_TextMargins.left, m_Panel.GetHeight() - m_TextMargins.top - fLargestLineAscender);
		break;

	case HYTEXT_ScaleBox: {
		float fDesiredWidth = m_Panel.GetWidth() - m_TextMargins.left - m_TextMargins.right;
		float fDesiredHeight = m_Panel.GetHeight() - m_TextMargins.bottom - m_TextMargins.top;

		// Scale all drawables to fit within the box
		float fScaleAmt = HyMath::Min(fDesiredWidth / m_vTextDimensions.x, fDesiredHeight / m_vTextDimensions.y);
		
		m_vTextDimensions *= fScaleAmt;
		fLargestLineAscender *= fScaleAmt;
		fLargestLineDescender *= fScaleAmt;
		fLineHeight *= fScaleAmt;

		for(auto pDrawable : m_DrawableList)
		{
			if(pDrawable->GetType() == HYTYPE_Sprite)
			{
				pDrawable->pos.Set(pDrawable->pos.Get() * fScaleAmt);
				pDrawable->pos.Offset(m_TextMargins.left, m_Panel.GetHeight() - m_TextMargins.top - fLargestLineAscender);
				pDrawable->scale.Set(pDrawable->scale.GetX() * fScaleAmt, pDrawable->scale.GetY() * fScaleAmt);
			}
			else
			{
				pDrawable->scale.SetAll(fScaleAmt);
				pDrawable->pos.Offset(m_TextMargins.left, m_Panel.GetHeight() - m_TextMargins.top - fLargestLineAscender);
			}

			if(m_uiAttribs & LABELATTRIB_Vertical)
				pDrawable->pos.Offset(0.0f, (fDesiredHeight - fLineHeight) * -0.5f);
		}
		break; }

	case HYTEXT_Vertical:
		break;
	}

	switch(m_eStackedAlignment)
	{
	case HYALIGN_Left:
		break;

	case HYALIGN_Center: {
		float fAvailableWidth = m_Panel.GetWidth() - m_TextMargins.left - m_TextMargins.right;
		if(m_vTextDimensions.x < fAvailableWidth)
		{
			for(auto pDrawable : m_DrawableList)
				pDrawable->pos.Offset((fAvailableWidth - m_vTextDimensions.x) * 0.5f, 0.0f);
		}
		break; }

	case HYALIGN_Right: {
		float fAvailableWidth = m_Panel.GetWidth() - m_TextMargins.left - m_TextMargins.right;
		if(m_vTextDimensions.x < fAvailableWidth)
		{
			for(auto pDrawable : m_DrawableList)
				pDrawable->pos.Offset(fAvailableWidth - m_vTextDimensions.x, 0.0f);
		}
		break; }

	case HYALIGN_Justify: // TODO: Implement
		HyLogError("HyRichText::AssembleDrawables() - HYALIGN_Justify not implemented");
		break;
	}

	// Inform everwhere that *this has been updated
	SetDirty(IHyNode::DIRTY_SceneAABB);
	//SetSizeAndLayoutDirty();
}
