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
    HyCamera2d *        m_pCamera;
    bool                m_bReloadDraw;

public:
    IDraw();

    void ProjLoad(IHyApplication &hyApp);
    void ProjUnload(IHyApplication &hyApp);
    void ProjShow(IHyApplication &hyApp);
    void ProjHide(IHyApplication &hyApp);
    void ProjUpdate(IHyApplication &hyApp);

    virtual void Relink(AtlasFrame *pFrame) { }

protected:
    virtual void OnPreLoad(IHyApplication &hyApp) = 0;
    virtual void OnPostUnload(IHyApplication &hyApp) = 0;
    virtual void OnProjShow(IHyApplication &hyApp) = 0;
    virtual void OnProjHide(IHyApplication &hyApp) = 0;
    virtual void OnProjUpdate(IHyApplication &hyApp) = 0;
};

#endif // IDRAW_H
