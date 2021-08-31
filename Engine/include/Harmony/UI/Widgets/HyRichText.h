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
public:
	HyRichText(HyEntity2d *pParent = nullptr);
	virtual ~HyRichText();

	virtual glm::ivec2 GetSizeHint() override;
	virtual glm::vec2 GetPosOffset() override;

	//void SetFormat(uint32 uiTextSize, uint32 uiLineSize, uint32 uiSpriteMaxHeight

	void InsertText(std::string sPrefix, std::string sName, std::string sText, uint32 uiState);
	void InsertSprite(std::string sPrefix, std::string sName, uint32 uiState);

protected:
	virtual void OnResize(int32 iNewWidth, int32 iNewHeight) override;
};

#endif /* HyRichText_h__ */
