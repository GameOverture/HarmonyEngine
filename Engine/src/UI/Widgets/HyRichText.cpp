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
#include "Diagnostics/Console/IHyConsole.h"

#include <regex>

HyRichText::HyRichText(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiColumnWidth(0),
	m_fTotalHeight(0.0f),
	m_fColumnLineHeightOffset(0.0f)
{
}

HyRichText::HyRichText(const std::string &sTextPrefix, const std::string &sTextName, uint32 uiColumnWidth, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiColumnWidth(0),
	m_fTotalHeight(0.0f),
	m_fColumnLineHeightOffset(0.0f)
{
	Setup(sTextPrefix, sTextName, uiColumnWidth);
}

/*virtual*/ HyRichText::~HyRichText()
{
	while(m_DrawableList.empty() == false)
	{
		delete m_DrawableList.back();
		m_DrawableList.pop_back();
	}
}

/*virtual*/ float HyRichText::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	return m_uiColumnWidth * fPercent;
}

/*virtual*/ float HyRichText::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	return m_fTotalHeight * fPercent;
}

void HyRichText::Setup(const std::string &sTextPrefix, const std::string &sTextName, uint32 uiColumnWidth)
{
	SetSizePolicy(HYSIZEPOLICY_Flexible, HYSIZEPOLICY_Fixed);

	m_sTextPrefix = sTextPrefix;
	m_sTextName = sTextName;
	m_uiColumnWidth = uiColumnWidth;

	AssembleDrawables();
}

// {1} = Any text inserted after this uses state '1'
// {spritePrefix/spriteName,3} = Insert a sprite ('spritePrefix/spriteName') with state '3', scaled to fit within text line
void HyRichText::SetRichText(const std::string &sRichTextFormat)
{
	m_sRichText = sRichTextFormat;
	AssembleDrawables();
}

/*virtual*/ glm::vec2 HyRichText::GetPosOffset() /*override*/
{
	return glm::vec2(0.0f, m_fTotalHeight - m_fColumnLineHeightOffset);
}

/*virtual*/ void HyRichText::OnSetSizeHint() /*override*/
{
	HySetVec(m_vSizeHint, m_uiColumnWidth, static_cast<int32>(m_fTotalHeight));
}

/*virtual*/ glm::ivec2 HyRichText::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	m_uiColumnWidth = uiNewWidth;
	AssembleDrawables();

	return glm::ivec2(m_uiColumnWidth, m_fTotalHeight);
}

void HyRichText::AssembleDrawables()
{
	// Clear out drawable list
	while(m_DrawableList.empty() == false)
	{
		delete m_DrawableList.back();
		m_DrawableList.pop_back();
	}
	m_fTotalHeight = 0.0f;
	m_fColumnLineHeightOffset = 0.0f;

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
	std::istringstream ssCleanText(std::regex_replace(m_sRichText, rgx, "\x7F")); // "\x7F" is delete character (127) to be used as a dilimiter to split each drawable

	// Reassemble the drawable list
	std::string sCurText;
	uint32 uiCurTextState = 0;
	glm::vec2 ptCurPos(0.0f, 0.0f);
	float fHorizontalCenterAccum = 0.0f;
	uint32 uiCurFmtIndex = 0;
	while(std::getline(ssCleanText, sCurText, '\x7F'))
	{
		HyText2d *pNewText = HY_NEW HyText2d(m_sTextPrefix, m_sTextName, this);
		pNewText->Load();
		m_DrawableList.push_back(pNewText);
		pNewText->pos.Set(0.0f, ptCurPos.y);
		pNewText->SetAsColumn(static_cast<float>(m_uiColumnWidth));
		//pNewText->SetTextAlignment(eAlignment);
		pNewText->SetState(uiCurTextState);
		pNewText->SetTextIndent(static_cast<uint32>(ptCurPos.x));
		pNewText->SetText(sCurText);

		// Update 'uiCurPos' to the location past the last glyph
		ptCurPos.x = pNewText->GetTextCursorPos().x;
		ptCurPos.y += pNewText->GetTextCursorPos().y;

		// TODO: 'm_fColumnLineHeightOffset' should actually be (I think) the max of first line's height, not the max line height overall
		const HyText2dData *pTextData = static_cast<const HyText2dData *>(pNewText->AcquireData());
		if(sCurText.empty() == false && pTextData)
			m_fColumnLineHeightOffset = HyMax(m_fColumnLineHeightOffset, pTextData->GetLineHeight(uiCurTextState));

		// Handle next format change
		if(uiCurFmtIndex < formatChangeList.size())
		{
			// If .first is empty string then just change text state
			if(formatChangeList[uiCurFmtIndex].first.empty())
				uiCurTextState = formatChangeList[uiCurFmtIndex].second;
			else // Otherwise insert sprite
			{
				// Using the text's data, determine how the line height used to scale the inserted sprite
				float fLineHeight = 32.0f; // Some default value to fallback to
				const HyText2dData *pTextData = static_cast<const HyText2dData *>(pNewText->AcquireData());
				if(pTextData)
					fLineHeight = pTextData->GetLineHeight(uiCurTextState);
				else
					HyLogWarning("HyRichText could not acquire data for text: " << pNewText->GetPrefix() << "/" << pNewText->GetName());

				// Assemble the prefix and name for this sprite
				std::string sName = formatChangeList[uiCurFmtIndex].first;
				std::string sPrefix;
				if(sName.find_last_of('/') != std::string::npos)
				{
					sPrefix = sName.substr(0, sName.find_last_of('/'));
					sName = sName.substr(sName.find_last_of('/') + 1);
				}

				// Allocate sprite and initialize
				HySprite2d *pNewSprite = HY_NEW HySprite2d(sPrefix, sName, this);
				pNewSprite->Load();
				m_DrawableList.push_back(pNewSprite);

				pNewSprite->SetState(formatChangeList[uiCurFmtIndex].second);

				// Determine sprite scale with remaining room left on line
				float fScaleX = (m_uiColumnWidth - ptCurPos.x) / pNewSprite->GetStateWidth(pNewSprite->GetState());
				float fScaleY = fLineHeight / pNewSprite->GetStateHeight(pNewSprite->GetState());
				pNewSprite->scale.Set(HyMin(fScaleX, fScaleY));

				// Determine if this sprite will not fit in the remaining space on this text line
				if((ptCurPos.x + pNewSprite->GetStateWidth(pNewSprite->GetState(), pNewSprite->scale.X())) >= (m_uiColumnWidth-1)) // the -1 should help with scale floating point above
				{
					ptCurPos.x = 0.0f;
					ptCurPos.y -= fLineHeight;

					// Recalculate sprite scale with modified ptCurPos
					float fScaleX = (m_uiColumnWidth - ptCurPos.x) / pNewSprite->GetStateWidth(pNewSprite->GetState());
					float fScaleY = fLineHeight / pNewSprite->GetStateHeight(pNewSprite->GetState());
					pNewSprite->scale.Set(HyMin(fScaleX, fScaleY));
				}
				pNewSprite->pos.Set(ptCurPos);

				// Find next drawable location and position 'ptCurPos' to it
				ptCurPos.x += pNewSprite->GetStateWidth(pNewSprite->GetState(), pNewSprite->scale.X());

				glm::vec2 vOffset = pNewSprite->GetCurFrameOffset();
				vOffset *= pNewSprite->scale.Get();
				pNewSprite->pos.Offset(-vOffset);	// NOTE: Only offsetting based on frame '0' - which will only work *most* of the time

				// Also offset the sprite down by the descender amount, because at the moment they're sitting on the 'baseline'
				pNewSprite->pos.Offset(0.0f, -abs(pTextData->GetLineDescender(uiCurTextState)));
			}

			uiCurFmtIndex++;
		}
	}

	m_fTotalHeight = std::fabs(ptCurPos.y);
	m_fTotalHeight += m_fColumnLineHeightOffset;// std::fabs(pTextData->GetLineDescender(uiCurTextState)

	SetDirty(IHyNode::DIRTY_BoundingVolume);
	m_bSizeHintDirty = true;
}
