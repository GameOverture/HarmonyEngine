#ifndef SPRITEDRAW_H
#define SPRITEDRAW_H

#include "IDraw.h"
#include "SpriteModels.h"

class SpriteDraw : public IDraw
{
    HySprite2d                              m_Sprite;

    HyPrimitive2d                           m_primOriginHorz;
    HyPrimitive2d                           m_primOriginVert;

public:
    SpriteDraw(ProjectItem *pProjItem, IHyApplication &hyApp);
    virtual ~SpriteDraw();

    virtual void OnApplyJsonData(jsonxx::Value &valueRef, bool bReloadInAssetManager) override;
    void SetFrame(quint32 uiStateIndex, quint32 uiFrameIndex);

protected:
    virtual void OnShow(IHyApplication &hyApp) override;
    virtual void OnHide(IHyApplication &hyApp) override;
    virtual void OnResizeRenderer() override;
    
    virtual void OnUpdate() override;
};

#endif // SPRITEDRAW_H
