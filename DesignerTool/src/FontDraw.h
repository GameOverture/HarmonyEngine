#ifndef FONTDRAW_H
#define FONTDRAW_H

#include "IDraw.h"
#include "FontItem.h"
#include "FontModels.h"
#include <QList>

class FontDraw : public IDraw
{
    bool                        m_bShowAtlasPreview;

    HyCamera2d *                m_pAtlasCamera;
    HyTexturedQuad2d *          m_pDrawAtlasPreview;
    HyPrimitive2d               m_DrawAtlasOutline;

    HyPrimitive2d               m_DividerLine;

    HyText2d                    m_Text;

public:
    FontDraw(ProjectItem *pProjItem, IHyApplication &hyApp);
    virtual ~FontDraw();

    virtual void OnApplyJsonData(jsonxx::Value &valueRef, bool bReloadInAssetManager) override;
    
    void PositionDividerLine();
    
    void ShowSubAtlas(bool bShow);
    
    void LoadNewAtlas(texture_atlas_t *pAtlas, unsigned char *pAtlasPixelData, uint uiAtlasPixelDataSize);
    void GenerateTextPreview(FontLayersModel *pFontModel, QString sFontPreviewString, texture_atlas_t *pAtlas);

protected:
    virtual void OnShow(IHyApplication &hyApp) override;
    virtual void OnHide(IHyApplication &hyApp) override;
    
    virtual void OnUpdate() override;
};

#endif // FONTDRAW_H
