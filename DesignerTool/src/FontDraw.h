#ifndef FONTDRAW_H
#define FONTDRAW_H

#include "IDraw.h"
#include "FontItem.h"
#include <QList>

class FontDraw : public IDraw
{
    FontItem *                  m_pItem;

    HyTexturedQuad2d *          m_pDrawAtlasPreview;
    HyPrimitive2d               m_DrawAtlasOutline;
    HyCamera2d *                m_pFontCamera;

    HyPrimitive2d               m_DividerLine;
    QList<HyTexturedQuad2d *>   m_DrawFontPreviewList;

public:
    FontDraw(FontItem *pItem);
    virtual ~FontDraw();

protected:
    virtual void OnGuiLoad(IHyApplication &hyApp) override;
    virtual void OnGuiUnload(IHyApplication &hyApp) override;
    virtual void OnGuiShow(IHyApplication &hyApp) override;
    virtual void OnGuiHide(IHyApplication &hyApp) override;
    virtual void OnGuiUpdate(IHyApplication &hyApp) override;
};

#endif // FONTDRAW_H
