/**************************************************************************
 *	ItemFont.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMFONT_H
#define ITEMFONT_H

#include "ItemWidget.h"
#include "freetype-gl/freetype-gl.h"

class ItemFont : public ItemWidget
{
    Q_OBJECT

    HyTexturedQuad2d *          m_pDrawAtlasPreview;
    HyPrimitive2d               m_DrawAtlasOutline;
    HyCamera2d *                m_pFontCamera;
    
    HyPrimitive2d               m_DividerLine;
    QList<HyTexturedQuad2d *>   m_DrawFontPreviewList;
    
    
public:
    ItemFont(ItemProject *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~ItemFont();
    
protected:
    virtual void OnGiveMenuActions(QMenu *pMenu);

    virtual void OnGuiLoad(IHyApplication &hyApp);
    virtual void OnGuiUnload(IHyApplication &hyApp);
    virtual void OnGuiShow(IHyApplication &hyApp);
    virtual void OnGuiHide(IHyApplication &hyApp);
    virtual void OnGuiUpdate(IHyApplication &hyApp);

    virtual void OnLink(HyGuiFrame *pFrame);
    virtual void OnReLink(HyGuiFrame *pFrame);
    virtual void OnUnlink(HyGuiFrame *pFrame);
    
    virtual QJsonValue OnSave();
};

#endif // ITEMFONT_H
