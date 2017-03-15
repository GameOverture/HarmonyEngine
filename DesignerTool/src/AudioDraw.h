#ifndef AUDIODRAW_H
#define AUDIODRAW_H

#include "IDraw.h"
#include "AudioItem.h"

class AudioDraw : public IDraw
{
    AudioItem *     m_pItem;

public:
    AudioDraw(AudioItem *pItem);
    virtual ~AudioDraw();

protected:
    virtual void OnProjLoad(IHyApplication &hyApp) override;
    virtual void OnProjUnload(IHyApplication &hyApp) override;
    virtual void OnProjShow(IHyApplication &hyApp) override;
    virtual void OnProjHide(IHyApplication &hyApp) override;
    virtual void OnProjUpdate(IHyApplication &hyApp) override;
};

#endif // AUDIODRAW_H
