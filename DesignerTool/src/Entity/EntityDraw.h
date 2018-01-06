/**************************************************************************
*	EntityDraw.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYDRAW_H
#define ENTITYDRAW_H

#include "IDraw.h"

class EntityDraw : public IDraw
{
public:
	EntityDraw(ProjectItem *pProjItem, IHyApplication &hyApp);
	virtual ~EntityDraw();

protected:
	virtual void OnApplyJsonData(jsonxx::Value &valueRef) override;
	virtual void OnShow(IHyApplication &hyApp) override;
	virtual void OnHide(IHyApplication &hyApp) override;
	virtual void OnResizeRenderer() override;
};

#endif // ENTITYDRAW_H
