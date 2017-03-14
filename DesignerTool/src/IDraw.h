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

class IDraw : public HyEntity2d
{
public:
    IDraw();

protected:
    virtual void OnGuiLoad(IHyApplication &hyApp) = 0;
    virtual void OnGuiUnload(IHyApplication &hyApp) = 0;
    virtual void OnGuiShow(IHyApplication &hyApp) = 0;
    virtual void OnGuiHide(IHyApplication &hyApp) = 0;
    virtual void OnGuiUpdate(IHyApplication &hyApp) = 0;

private:
    void Load(IHyApplication &hyApp);
    void Unload(IHyApplication &hyApp);
    void DrawShow(IHyApplication &hyApp);
    void DrawHide(IHyApplication &hyApp);
    void DrawUpdate(IHyApplication &hyApp);
};

#endif // IDRAW_H
