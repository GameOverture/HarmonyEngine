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

class ItemFont : public ItemWidget
{
    Q_OBJECT
    
    struct tGlyphSet
    {
        QString     sFontName;
        int         iSize;
        QString     sAvailChars;
        
        // Fill colors
        QColor      clrTopLeft;
        QColor      clrTopRight;
        QColor      clrBotLeft;
        QColor      clrBotRight;
        
        struct tDrawInfo
        {
            int     iType;  // 0 - Fill, 1 - Line, 2 - Inner, 3 - Outer
            float   fThickness;
        };
        
        QList<tDrawInfo>    drawOrder;
    };
    
    struct tFontMap
    {
        int                 iAtlasWidth;
        int                 iAtlasHeight;
        
        QList<tGlyphSet>    glyphSets;
    };
    
    
public:
    ItemFont(const QString sPath, WidgetAtlasManager &atlasManRef);
    virtual ~ItemFont();
    
    virtual QList<QAction *> GetActionsForToolBar();
    
protected:
    virtual void OnLoad(IHyApplication &hyApp);
    virtual void OnUnload(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);

    virtual void OnLink(HyGuiFrame *pFrame);
    virtual void OnReLink(HyGuiFrame *pFrame);
    virtual void OnUnlink(HyGuiFrame *pFrame);
    
    virtual void OnSave();
};

#endif // ITEMFONT_H
