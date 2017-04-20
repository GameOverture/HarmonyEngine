#ifndef FONTDRAW_H
#define FONTDRAW_H

#include "IDraw.h"
#include "FontItem.h"
#include "FontModels.h"
#include <QList>

class FontDraw : public IDraw
{
    FontModel &                 m_ModelRef;
    
    bool                        m_bShowAtlasPreview;

    HyCamera2d *                m_pAtlasCamera;
    HyTexturedQuad2d *          m_pDrawAtlasPreview;
    HyPrimitive2d               m_DrawAtlasOutline;

    HyPrimitive2d               m_DividerLine;
    QList<HyTexturedQuad2d *>   m_DrawFontPreviewList;

public:
    FontDraw(FontModel &modelRef, IHyApplication &hyApp);
    virtual ~FontDraw();
    
    void PositionDividerLine();
    
    void ShowSubAtlas(bool bShow);
    
    void LoadNewAtlas(texture_atlas_t *pAtlas, unsigned char *pAtlasPixelData);
    void GenerateTextPreview(FontTableModel *pFontModel, QString sFontPreviewString, texture_atlas_t *pAtlas);

protected:
    virtual void OnShow(IHyApplication &hyApp) override;
    virtual void OnHide(IHyApplication &hyApp) override;
};

#endif // FONTDRAW_H
