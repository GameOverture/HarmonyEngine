#include "EntityDraw.h"

EntityDraw::EntityDraw(ProjectItem *pProjItem, IHyApplication &hyApp) : IDraw(pProjItem, hyApp)
{

}

/*virtual*/ EntityDraw::~EntityDraw()
{
}

/*virtual*/ void EntityDraw::OnApplyJsonData(jsonxx::Value &valueRef, bool bReloadInAssetManager) /*override*/
{
}

/*virtual*/ void EntityDraw::OnShow(IHyApplication &hyApp) /*override*/
{
}

/*virtual*/ void EntityDraw::OnHide(IHyApplication &hyApp) /*override*/
{
}

/*virtual*/ void EntityDraw::OnResizeRenderer() /*override*/
{
}
