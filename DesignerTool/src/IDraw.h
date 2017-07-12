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
#include <QWidget>

class IDraw : public HyEntity2d
{
protected:
    IHyApplication &    m_HyAppRef;
    HyCamera2d *        m_pCamera;

public:
    IDraw(IHyApplication &hyApp);
    virtual ~IDraw();

    void Show();
    void Hide();

protected:
    virtual void OnShow(IHyApplication &hyApp) = 0;
    virtual void OnHide(IHyApplication &hyApp) = 0;
    
    virtual void WidgetUpdate(QWidget *pWidget) = 0;
};

#endif // IDRAW_H
