/**************************************************************************
 *	IDraw.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IDRAW_H
#define IDRAW_H

#include "Harmony/HyEngine.h"
#include "Harmony/Utilities/jsonxx.h"

#include <QWidget>

class ProjectItem;

class IDraw : public HyEntity2d
{
protected:
    ProjectItem *       m_pProjItem;
    IHyApplication &    m_HyAppRef;
    HyCamera2d *        m_pCamera;

public:
    IDraw(ProjectItem *pProjItem, IHyApplication &hyApp);
    virtual ~IDraw();
    
    void ApplyJsonData();

    void Show();
    void Hide();

protected:
    virtual void OnApplyJsonData(jsonxx::Value &valueRef) { }
    virtual void OnShow(IHyApplication &hyApp) = 0;
    virtual void OnHide(IHyApplication &hyApp) = 0;
};

#endif // IDRAW_H
