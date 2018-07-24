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
	HyCamera2d *                m_pPreviewTextCamera;
	HyTexturedQuad2d *          m_pAtlasPreviewTexQuad;
	HyPrimitive2d               m_DrawAtlasOutline;

	HyPrimitive2d               m_DividerLine;
	HyPrimitive2d               m_PreviewOriginHorz;

	HyText2d                    m_Text;

public:
	FontDraw(ProjectItem *pProjItem, IHyApplication &hyApp);
	virtual ~FontDraw();

	virtual void OnApplyJsonData(jsonxx::Value &valueRef) override;

protected:
	virtual void OnShow(IHyApplication &hyApp) override;
	virtual void OnHide(IHyApplication &hyApp) override;
	virtual void OnResizeRenderer() override;
	
	virtual void OnUpdate() override;
};

#endif // FONTDRAW_H
