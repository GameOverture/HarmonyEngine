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
	enum RichTextAttributes
	{
		RICHTEXTATTRIB_IsCenterVertically = 1 << 16,

		RICHTEXTATTRIB_TextTypeMask = 0xE0000,			// When Panel is 'Stacked' (default), this mask holds HyTextType enum value.
		RICHTEXTATTRIB_TextTypeOffset = 17,				// Bit shift offset to get/set 'HyTextType'
		//							  = 1 << 17
		//							  = 1 << 18			// 3 bits, bit's 17-19
		//							  = 1 << 19

		RICHTEXTATTRIB_NEXTFLAG = 1 << 20
	};
	static_assert((int)RICHTEXTATTRIB_IsCenterVertically == (int)WIDGETATTRIB_NEXTFLAG, "HyRichText is not matching with base classes attrib flags");

	HyNodePath						m_TextPath;
	
	HyMargins<float>				m_TextMargins;
	glm::vec2						m_vBoxDimensions;
	HyAlignment						m_eAlignment;
	float							m_fColumnLineHeightOffset;

	std::string						m_sRichText;
	std::vector<IHyDrawable2d *>	m_DrawableList;

public:
	HyRichText(HyEntity2d *pParent = nullptr);
	HyRichText(const HyPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	HyRichText(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent = nullptr);
	HyRichText(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent = nullptr);
	virtual ~HyRichText();

	bool IsGlyphAvailable(std::string sUtf8Character) const;

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	void Setup(const HyPanelInit &panelInit);
	void Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath);
	void Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins);

	HyTextType GetTextType() const;
	void SetAsLine();
	void SetAsColumn(float fWidth);
	void SetAsBox(float fWidth, float fHeight, bool bCenterVertically);
	void SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically = true);

	bool IsCenterVertically() const;

	HyAlignment GetAlignment() const;
	void SetAlignment(HyAlignment eAlignment);

	// Formatting examples:
	// {1} = All text after this uses state 1
	// {spritePrefix/spriteName,3} = Insert a sprite "spritePrefix/spriteName" with state 3, scaled to fit within text line
	void SetText(const std::string &sRichTextFormat);

protected:
	virtual glm::vec2 GetPosOffset() override;
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	void AssembleDrawables();

	virtual void OnSetup() { }					// Optional override for derived classes
};

#endif /* HyRichText_h__ */
