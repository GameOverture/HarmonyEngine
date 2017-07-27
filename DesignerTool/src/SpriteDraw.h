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

    void SetFrame(quint32 uiStateIndex, quint32 uiFrameIndex);

    virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
    virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;
    virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
    virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;
    virtual void OnMouseWheelEvent(QWheelEvent *pEvent) override;
    virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;

protected:
    virtual void OnApplyJsonData(jsonxx::Value &valueRef, bool bReloadInAssetManager) override;
    virtual void OnShow(IHyApplication &hyApp) override;
    virtual void OnHide(IHyApplication &hyApp) override;
    virtual void OnResizeRenderer() override;
    
    virtual void OnUpdate() override;
};

#endif // SPRITEDRAW_H
