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

struct FontStagePass
{
    int                 iReferenceCount;
    int                 iTmpReferenceCount;

    texture_font_t *    pTextureFont;
    rendermode_t        eMode;
    float               fSize;
    float               fOutlineThickness;

    FontStagePass(rendermode_t eRenderMode, float fSize, float fOutlineThickness) : iReferenceCount(0),
                                                                                    iTmpReferenceCount(0),
                                                                                    pTextureFont(NULL),
                                                                                    eMode(eRenderMode),
                                                                                    fSize(fSize),
                                                                                    fOutlineThickness(fOutlineThickness)
    { }

    ~FontStagePass()
    {
        if(pTextureFont)
            texture_font_delete(pTextureFont);
    }

    void SetFont(texture_font_t *pNewFont)
    {
        if(pTextureFont)
            texture_font_delete(pTextureFont);

        pTextureFont = pNewFont;
        pTextureFont->size = fSize;
        pTextureFont->rendermode = eMode;
        pTextureFont->outline_thickness = fOutlineThickness;
    }
};

struct FontLayer
{
    const int           iUNIQUE_ID;
    
    rendermode_t        eMode;
    float               fSize;
    float               fOutlineThickness;
    
    FontLayer(int iUniqueId, rendermode_t eRenderMode, float fSize, float fOutlineThickness) :  iUNIQUE_ID(iUniqueId),
                                                                                                eMode(eRenderMode),
                                                                                                fSize(fSize),
                                                                                                fOutlineThickness(fOutlineThickness)
    { }
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
