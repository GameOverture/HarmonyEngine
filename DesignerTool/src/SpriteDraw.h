#ifndef SPRITEDRAW_H
#define SPRITEDRAW_H

#include "IDraw.h"
#include "SpriteModels.h"

class SpriteDraw : public IDraw
{
    SpriteModel &                           m_ModelRef;

    QMap<quint32, HyTexturedQuad2d *>       m_TexturedQuadMap;
    HyTexturedQuad2d *                      m_pCurFrame;

    HyPrimitive2d                           m_primOriginHorz;
    HyPrimitive2d                           m_primOriginVert;

public:
    SpriteDraw(SpriteModel &modelRef, IHyApplication &hyApp);
    virtual ~SpriteDraw();

    //virtual void Relink(AtlasFrame *pFrame) override;

    void SetFrame(quint32 uiChecksum, glm::vec2 vOffset);

protected:
    virtual void OnShow(IHyApplication &hyApp) override;
    virtual void OnHide(IHyApplication &hyApp) override;
};

#endif // SPRITEDRAW_H
