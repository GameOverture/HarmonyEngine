/**************************************************************************
*	HyRichLabel.h
*
*	Harmony Engine
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyRichLabel_h__
#define HyRichLabel_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"

class HyRichLabel : public HyLabel
{
protected:
	std::string										m_sRichText;
	glm::vec2										m_vTextDimensions;

	std::vector<IHyDrawable2d *>					m_DrawableList;

public:
	HyRichLabel(HyEntity2d *pParent = nullptr);
	HyRichLabel(const HyPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	HyRichLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent = nullptr);
	HyRichLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent = nullptr);
	virtual ~HyRichLabel();

	virtual float GetTextWidth(float fPercent = 1.0f) override;		// Includes formatted sprites
	virtual float GetTextHeight(float fPercent = 1.0f) override;	// Includes formatted sprites

	virtual std::string GetUtf8String() const override;

	// Formatting examples:
	// {1} = All text after this uses state 1
	// {spritePrefix/spriteName,3} = Insert a sprite "spritePrefix/spriteName" with state 3, scaled to fit within text line
	virtual void SetText(const std::string &sUtf8RichText) override;

	void ForEachDrawable(std::function<void(IHyDrawable2d *)> fpForEachDrawable);

protected:
	virtual void OnAssemble() override;

private:
	using HyLabel::GetTextState;
	using HyLabel::SetTextState;

	using HyLabel::GetLineBreakHeight;
	using HyLabel::GetLineDescender;
	using HyLabel::GetGlyphOffset;
	using HyLabel::GetGlyphSize;

	using HyLabel::GetNumCharacters;

	using HyLabel::GetCharacterCode;
	using HyLabel::GetCharacterOffset;
	using HyLabel::SetCharacterOffset;
	using HyLabel::GetCharacterScale;
	using HyLabel::SetCharacterScale;
	using HyLabel::GetCharacterAlpha;
	using HyLabel::SetCharacterAlpha;

	using HyLabel::GetNumLayers;

	using HyLabel::GetLayerColor;
	using HyLabel::SetTextLayerColor;
};

#endif /* HyRichLabel_h__ */
