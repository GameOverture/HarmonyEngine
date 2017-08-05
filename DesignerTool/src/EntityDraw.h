#ifndef ENTITYDRAW_H
#define ENTITYDRAW_H

#include "IDraw.h"

class EntityDraw : public IDraw
{
public:
    EntityDraw(ProjectItem *pProjItem, IHyApplication &hyApp);
    virtual ~EntityDraw();

protected:
    virtual void OnApplyJsonData(jsonxx::Value &valueRef, bool bReloadInAssetManager) override;
    virtual void OnShow(IHyApplication &hyApp) override;
    virtual void OnHide(IHyApplication &hyApp) override;
    virtual void OnResizeRenderer() override;
};

#endif // ENTITYDRAW_H
