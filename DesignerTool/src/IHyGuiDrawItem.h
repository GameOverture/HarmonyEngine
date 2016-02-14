#ifndef IHYGUIDRAWITEM
#define IHYGUIDRAWITEM

#include "Harmony/HyEngine.h"

class IHyGuiDrawItem
{
protected:
    virtual void Show(IHyApplication &hyApp) = 0;
    virtual void Hide(IHyApplication &hyApp) = 0;

    virtual void Draw(IHyApplication &hyApp) = 0;
};

#endif // IHYGUIDRAWITEM

