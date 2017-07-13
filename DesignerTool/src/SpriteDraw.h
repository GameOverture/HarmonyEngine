#ifndef SPRITEDRAW_H
#define SPRITEDRAW_H

#include "IDraw.h"
#include "SpriteModels.h"

class SpriteDraw : public IDraw
{
    QMap<quint32, HyTexturedQuad2d *>       m_TexturedQuadIdMap;
    HyTexturedQuad2d *                      m_pCurFrame;

    HyPrimitive2d                           m_primOriginHorz;
    HyPrimitive2d                           m_primOriginVert;

public:
    SpriteDraw(ProjectItem *pProjItem, IHyApplication &hyApp);
    virtual ~SpriteDraw();

    void SetFrame(quint32 uiId, glm::vec2 vOffset);

protected:
    virtual void OnShow(IHyApplication &hyApp) override;
    virtual void OnHide(IHyApplication &hyApp) override;
};

#endif // SPRITEDRAW_H
