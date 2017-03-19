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

class AtlasFrame;

class IDraw : public HyEntity2d
{
protected:
    IHyApplication &    m_HyAppRef;
    HyCamera2d *        m_pCamera;

public:
    IDraw(IHyApplication &hyApp);
    virtual ~IDraw();

    void ProjShow();
    void ProjHide();
    void ProjUpdate();

protected:
    virtual void OnProjShow(IHyApplication &hyApp) = 0;
    virtual void OnProjHide(IHyApplication &hyApp) = 0;
    virtual void OnProjUpdate(IHyApplication &hyApp) = 0;
};

#endif // IDRAW_H
