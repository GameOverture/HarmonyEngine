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
    virtual void OnProjLoad(IHyApplication &hyApp) override;
    virtual void OnProjUnload(IHyApplication &hyApp) override;
    virtual void OnProjShow(IHyApplication &hyApp) override;
    virtual void OnProjHide(IHyApplication &hyApp) override;
    virtual void OnProjUpdate(IHyApplication &hyApp) override;
};

#endif // FONTDRAW_H
