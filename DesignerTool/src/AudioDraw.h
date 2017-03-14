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
    virtual void OnGuiLoad(IHyApplication &hyApp) override;
    virtual void OnGuiUnload(IHyApplication &hyApp) override;
    virtual void OnGuiShow(IHyApplication &hyApp) override;
    virtual void OnGuiHide(IHyApplication &hyApp) override;
    virtual void OnGuiUpdate(IHyApplication &hyApp) override;
};

#endif // AUDIODRAW_H
