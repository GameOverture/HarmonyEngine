#ifndef IHYGUIDRAWITEM
#define IHYGUIDRAWITEM

#include "Harmony/HyEngine.h"

class IHyGuiDrawItem
{
protected:
    virtual void Load(IHyApplication &hyApp) = 0;
    virtual void Unload() = 0;

    virtual void Show() = 0;
    virtual void Hide() = 0;

    virtual void Draw(IHyApplication &hyApp) = 0;
};

#endif // IHYGUIDRAWITEM

