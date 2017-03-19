#ifndef FONTDRAW_H
#define FONTDRAW_H

#include "IDraw.h"
#include "FontItem.h"
#include <QList>

class FontDraw : public IDraw
{
    //FontModel &                 m_ModelRef;

    HyTexturedQuad2d *          m_pDrawAtlasPreview;
    HyPrimitive2d               m_DrawAtlasOutline;
    HyCamera2d *                m_pFontCamera;

    HyPrimitive2d               m_DividerLine;
    QList<HyTexturedQuad2d *>   m_DrawFontPreviewList;

public:
    FontDraw(IHyApplication &hyApp);
    virtual ~FontDraw();

protected:
    virtual void OnPreLoad(IHyApplication &hyApp) ;
    virtual void OnPostUnload(IHyApplication &hyApp) ;
    virtual void OnProjShow(IHyApplication &hyApp) override;
    virtual void OnProjHide(IHyApplication &hyApp) override;
    virtual void OnProjUpdate(IHyApplication &hyApp) override;
};

#endif // FONTDRAW_H
