/**************************************************************************
*	EntityDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYDRAW_H
#define ENTITYDRAW_H

#include "IDraw.h"
#include "EntityModel.h"

class EntityDraw : public IDraw
{
public:
	EntityDraw(ProjectItem *pProjItem, IHyApplication &hyApp);
	virtual ~EntityDraw();

	void Sync();

protected:
	virtual void OnApplyJsonData(jsonxx::Value &valueRef) override;
	virtual void OnShow(IHyApplication &hyApp) override;
	virtual void OnHide(IHyApplication &hyApp) override;
	virtual void OnResizeRenderer() override;
};

#endif // ENTITYDRAW_H
