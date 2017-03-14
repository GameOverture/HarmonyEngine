/**************************************************************************
 *	ItemFont.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTDATA_H
#define FONTDATA_H

#include "IData.h"
#include "freetype-gl/freetype-gl.h"

class FontData : public IData
{
    Q_OBJECT

    HyTexturedQuad2d *          m_pDrawAtlasPreview;
    HyPrimitive2d               m_DrawAtlasOutline;
    HyCamera2d *                m_pFontCamera;
    
    HyPrimitive2d               m_DividerLine;
    QList<HyTexturedQuad2d *>   m_DrawFontPreviewList;
    
    
public:
    FontData(Project *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~FontData();
    
protected:
    virtual void OnGiveMenuActions(QMenu *pMenu);

    virtual void OnGuiLoad(IHyApplication &hyApp);
    virtual void OnGuiUnload(IHyApplication &hyApp);
    virtual void OnGuiShow(IHyApplication &hyApp);
    virtual void OnGuiHide(IHyApplication &hyApp);
    virtual void OnGuiUpdate(IHyApplication &hyApp);

    virtual void OnLink(AtlasFrame *pFrame);
    virtual void OnReLink(AtlasFrame *pFrame);
    virtual void OnUnlink(AtlasFrame *pFrame);
    
    virtual QJsonValue OnSave();
};

#endif // FONTDATA_H
