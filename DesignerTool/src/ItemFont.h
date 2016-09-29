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

#include <QColor>

class FontStage
{
public:
    enum eType
    {
        TYPE_Normal = 0,
        TYPE_OutlineEdge,
        TYPE_OutlinePositive,
        TYPE_OutlineNegative,
        TYPE_DistanceField
    };

private:
    eType               m_eType;
    float               m_fSize;
    float               m_fOutlineThickness;

    QColor              m_TopColor;
    QColor              m_BotColor;

    texture_font_t *    m_pTextureFont;

public:
    FontStage(eType eRenderType, float fSize, float fOutlineThickness, QColor topColor, QColor botColor);

    QString GetTypeString();

    float GetThickness();

    float GetSize();

    void SetNewTextureFont(texture_font_t *pNewFont);
};

class ItemFont : public ItemWidget
{
    Q_OBJECT
    
    HyTexturedQuad2d *      m_pDrawPreview;
    
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
