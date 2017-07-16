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

    virtual void ApplyJsonData(QJsonValue &valueData) override;
    void SetFrame(quint32 uiId, glm::vec2 vOffset);

protected:
    virtual void OnShow(IHyApplication &hyApp) override;
    virtual void OnHide(IHyApplication &hyApp) override;
};

#endif // SPRITEDRAW_H
