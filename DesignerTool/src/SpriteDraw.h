#ifndef SPRITEDRAW_H
#define SPRITEDRAW_H

#include "IDraw.h"
#include "SpriteItem.h"

class SpriteDraw : public IDraw
{
    SpriteItem *                        m_pItem;

    QMap<quint32, HyTexturedQuad2d *>   m_FrameMap;
    HyTexturedQuad2d *                  m_pCurFrame;

    HyPrimitive2d                       m_primOriginHorz;
    HyPrimitive2d                       m_primOriginVert;

public:
    SpriteDraw(SpriteItem *pItem);
    virtual ~SpriteDraw();

protected:
    virtual void OnPreLoad(IHyApplication &hyApp) override;
    virtual void OnPostUnload(IHyApplication &hyApp) override;
    virtual void OnProjShow(IHyApplication &hyApp) override;
    virtual void OnProjHide(IHyApplication &hyApp) override;
    virtual void OnProjUpdate(IHyApplication &hyApp) override;
};

#endif // SPRITEDRAW_H
