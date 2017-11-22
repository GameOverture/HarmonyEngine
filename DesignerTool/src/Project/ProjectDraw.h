/**************************************************************************
 *	ProjectDraw.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECTDRAW_H
#define PROJECTDRAW_H

#include "IDraw.h"

class CheckerGrid : public HyPrimitive2d
{
    glm::vec2		m_vDimensions;

public:
    CheckerGrid();
    virtual ~CheckerGrid();

    void SetDimensions(int iWidth, int iHeight);

    virtual void OnUpdateUniforms();
    virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

class ProjectDraw : public IDraw
{
    CheckerGrid         m_CheckerGrid;

public:
    ProjectDraw(IHyApplication &hyApp);
    virtual ~ProjectDraw();
    
    virtual void OnShow(IHyApplication &hyApp) override;
    virtual void OnHide(IHyApplication &hyApp) override;
    virtual void OnResizeRenderer() override;
};

#endif // PROJECTDRAW_H
