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
	m_fColumnLineHeightOffset(0.0f)
{
}

HyRichText::HyRichText(const std::string &sTextPrefix, const std::string &sTextName, uint32 uiColumnWidth, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiColumnWidth(0),
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

/*virtual*/ glm::ivec2 HyRichText::GetSizeHint() /*override*/
{
	return glm::ivec2(m_uiColumnWidth, GetSceneHeight() + m_fColumnLineHeightOffset);
}

/*virtual*/ glm::vec2 HyRichText::GetPosOffset() /*override*/
{
	return glm::vec2(0.0f, GetSceneHeight() + m_fColumnLineHeightOffset);
}

void HyRichText::Setup(const std::string &sTextPrefix, const std::string &sTextName, uint32 uiColumnWidth)
{
	m_SizePolicies[HYORIEN_Vertical] = HYSIZEPOLICY_Fixed;

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

/*virtual*/ void HyRichText::OnResize(int32 iNewWidth, int32 iNewHeight) /*override*/
{
	m_uiColumnWidth = iNewWidth;
	AssembleDrawables();
}

void HyRichText::AssembleDrawables()
{
	// Clear out drawable list
	while(m_DrawableList.empty() == false)
	{
		delete m_DrawableList.back();
		m_DrawableList.pop_back();
	}
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
			std::string sPath = m[1].str().substr(0, sPath.find(','));
			uint32 uiState = std::stoi(m[1].str().substr(sPath.find(',') + 1));

			formatChangeList.push_back(std::pair<std::string, uint32>(sPath, uiState));
		}
		else if(std::regex_search(sMatch, m, std::regex("([A-Za-z_]+[A-Za-z0-9_/]*)"))) // Doesn't start with number (sprite)
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
	uint32 uiCurFmtIndex = 0;
	while(std::getline(ssCleanText, sCurText, '\x7F'))
	{
		HyText2d *pNewText = HY_NEW HyText2d(m_sTextPrefix, m_sTextName, this);
		pNewText->Load();
		m_DrawableList.push_back(pNewText);
		pNewText->pos.Set(0.0f, ptCurPos.y);
		pNewText->SetAsColumn(m_uiColumnWidth);
		pNewText->SetState(uiCurTextState);
		pNewText->SetTextIndent(ptCurPos.x);
		pNewText->SetText(sCurText);

		// Update 'uiCurPos' to the location past the last glyph
		ptCurPos = pNewText->GetTextCursorPos();

		const HyText2dData *pTextData = static_cast<const HyText2dData *>(pNewText->AcquireData());
		if(sCurText.empty() == false && pTextData)
		{
			m_fColumnLineHeightOffset = 0.0f;// HyMax(m_fColumnLineHeightOffset, pTextData->GetLineHeight(uiCurTextState));
			ptCurPos.y += m_fColumnLineHeightOffset;
		}

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
				pNewSprite->pos.Set(ptCurPos);
				pNewSprite->SetState(formatChangeList[uiCurFmtIndex].second);

				float fScaleX = (m_uiColumnWidth - ptCurPos.x) / pNewSprite->GetStateMaxWidth(pNewSprite->GetState(), false);
				float fScaleY = fLineHeight / pNewSprite->GetStateMaxHeight(pNewSprite->GetState(), false);
				pNewSprite->scale.Set(HyMin(fScaleX, fScaleY));

				// Find next drawable location and position 'ptCurPos' to it
				ptCurPos.x += pNewSprite->GetStateMaxWidth(pNewSprite->GetState(), true);

				if(ptCurPos.x >= m_uiColumnWidth)
				{
					ptCurPos.x = 0.0f;
					ptCurPos.y -= fLineHeight;
				}
			}

			uiCurFmtIndex++;
		}
	}
}
