#ifndef SPRITEDRAW_H
#define SPRITEDRAW_H

#include "IDraw.h"
#include "SpriteModels.h"

class SpriteDraw : public IDraw
{
    SpriteModel &                           m_ModelRef;

    class AnimState : public HyEntity2d
    {
    public:
        QMap<quint32, HyTexturedQuad2d *>   m_FrameMap;
        HyTexturedQuad2d *                  m_pCurFrame;

        AnimState() : m_pCurFrame(nullptr)
        { }

        ~AnimState()
        {
            for(auto iter = m_FrameMap.begin(); iter != m_FrameMap.end(); ++iter)
                delete iter.value();
        }
    };
    QList<AnimState *>                      m_AnimStates;
    int                                     m_iCurStateIndex;

    HyPrimitive2d                           m_primOriginHorz;
    HyPrimitive2d                           m_primOriginVert;

public:
    SpriteDraw(SpriteModel &modelRef, IHyApplication &hyApp);
    virtual ~SpriteDraw();

    //virtual void Relink(AtlasFrame *pFrame) override;

protected:
    virtual void OnProjShow(IHyApplication &hyApp) override;
    virtual void OnProjHide(IHyApplication &hyApp) override;
    virtual void OnProjUpdate(IHyApplication &hyApp) override;
};

#endif // SPRITEDRAW_H
