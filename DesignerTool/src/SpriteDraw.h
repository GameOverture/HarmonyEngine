#ifndef SPRITEDRAW_H
#define SPRITEDRAW_H

#include "IDraw.h"
#include "SpriteItem.h"

class SpriteDraw : public IDraw
{
    SpriteItem *                m_pItem;

    HyPrimitive2d               m_primOriginHorz;
    HyPrimitive2d               m_primOriginVert;

public:
    SpriteDraw(SpriteItem *pItem);
    virtual ~SpriteDraw();

protected:
    virtual void OnProjLoad(IHyApplication &hyApp) override;
    virtual void OnProjUnload(IHyApplication &hyApp) override;
    virtual void OnProjShow(IHyApplication &hyApp) override;
    virtual void OnProjHide(IHyApplication &hyApp) override;
    virtual void OnProjUpdate(IHyApplication &hyApp) override;
};

#endif // SPRITEDRAW_H
