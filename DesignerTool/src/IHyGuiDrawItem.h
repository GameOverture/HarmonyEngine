/**************************************************************************
 *	IHyGuiDrawItem.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHYGUIDRAWITEM
#define IHYGUIDRAWITEM

#include "Harmony/HyEngine.h"

class IHyGuiDrawItem
{
    friend class WidgetRenderer;

    bool                m_bLoaded;

    void DrawOpen(IHyApplication &hyApp);
    void DrawClose(IHyApplication &hyApp);

    void DrawShow(IHyApplication &hyApp);
    void DrawHide(IHyApplication &hyApp);

    void DrawUpdate(IHyApplication &hyApp);

protected:
    HyCamera2d *        m_pCamera;
    IHyGuiDrawItem();

    virtual void OnDraw_Open(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Close(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Show(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Hide(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Update(IHyApplication &hyApp) = 0;
};

#endif // IHYGUIDRAWITEM

