/**************************************************************************
*	FontDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef FONTDRAW_H
#define FONTDRAW_H

#include "IDraw.h"
#include "FontModels.h"
#include <QList>

class FontDraw : public IDraw
{
	HyCamera2d *				m_pPreviewTextCamera;
	HyTexturedQuad2d *			m_pAtlasPreviewTexQuad;
	HyPrimitive2d				m_DrawAtlasOutline;

	HyPrimitive2d				m_PreviewOriginHorz;

	HyText2d					m_Text;

	float						m_fDividerLinePos;	// 0.0f -> 1.0f
	HyPrimitive2d				m_DividerLine;

public:
	FontDraw(ProjectItem *pProjItem);
	virtual ~FontDraw();

	void SetPreviewText(std::string sText);

	virtual void OnApplyJsonData(jsonxx::Value &valueRef) override;

protected:
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
	
	virtual void OnUpdate() override;
};

#endif // FONTDRAW_H
