/**************************************************************************
*	HyRichText.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyRichText_h__
#define HyRichText_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/IHyWidget.h"

class HyRichText : public IHyWidget
{
protected:
	HyNodePath						m_TextPath;
	uint32							m_uiColumnWidth;
	HyAlignment						m_eAlignment;

	float							m_fTotalHeight;
	float							m_fUsedWidth;
	float							m_fColumnLineHeightOffset;

	std::string						m_sRichText;
	std::vector<IHyDrawable2d *>	m_DrawableList;

public:
	HyRichText(HyEntity2d *pParent = nullptr);
	HyRichText(const HyNodePath &textNodePath, uint32 uiColumnWidth, HyAlignment eAlignment, HyEntity2d *pParent = nullptr);
	HyRichText(const HyPanelInit &panelInit, const HyNodePath &textNodePath, uint32 uiColumnWidth, HyAlignment eAlignment, HyEntity2d *pParent = nullptr);
	virtual ~HyRichText();

	bool IsGlyphAvailable(std::string sUtf8Character);

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	float GetTextWidth(float fPercent = 1.0f);

	void Setup(const HyNodePath &textNodePath, uint32 uiColumnWidth, HyAlignment eAlignment);
	void Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath, uint32 uiColumnWidth, HyAlignment eAlignment);
	uint32 GetColumnWidth() const;

	// {1} = Any text inserted after this uses state '1'
	// {spritePrefix/spriteName,3} = Insert a sprite ('spritePrefix/spriteName') with state '3', scaled to fit within text line
	void SetRichText(const std::string &sRichTextFormat);

protected:
	virtual glm::vec2 GetPosOffset() override;
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	void AssembleDrawables();
};

#endif /* HyRichText_h__ */
