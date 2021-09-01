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
	std::string						m_sTextPrefix;
	std::string						m_sTextName;
	uint32							m_uiColumnWidth;

	std::string						m_sRichText;
	std::vector<IHyDrawable2d *>	m_DrawableList;

public:
	HyRichText(HyEntity2d *pParent = nullptr);
	HyRichText(const std::string &sTextPrefix, const std::string &sTextName, uint32 uiColumnWidth, HyEntity2d *pParent = nullptr);
	virtual ~HyRichText();

	virtual glm::ivec2 GetSizeHint() override;
	virtual glm::vec2 GetPosOffset() override;

	void Setup(const std::string &sTextPrefix, const std::string &sTextName, uint32 uiColumnWidth);

	// {1} = Any text inserted after this uses state '1'
	// {spritePrefix/spriteName,3} = Insert a sprite ('spritePrefix/spriteName') with state '3', scaled to fit within text line
	void SetRichText(const std::string &sRichTextFormat);

protected:
	virtual void OnResize(int32 iNewWidth, int32 iNewHeight) override;

	void AssembleDrawables();
};

#endif /* HyRichText_h__ */