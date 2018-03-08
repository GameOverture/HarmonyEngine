/**************************************************************************
 *	ProjectDraw.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECTDRAW_H
#define PROJECTDRAW_H

#include "IDraw.h"

class CheckerGrid : public HyPrimitive2d
{
	const glm::vec2     m_vDIMENSIONS;
	float               m_fGridSize;

public:
	CheckerGrid(float fWidth, float fHeight, float fGridSize, HyEntity2d *pParent);
	virtual ~CheckerGrid();

	virtual void OnUpdateUniforms();
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

class ProjectDraw : public IDraw
{
	//CheckerGrid		m_CheckerGrid;
	HyShader *			m_pCheckerGridShader;
	HyPrimitive2d		m_TempGrid;

public:
	ProjectDraw(IHyApplication &hyApp);
	virtual ~ProjectDraw();
	
	virtual void OnShow(IHyApplication &hyApp) override;
	virtual void OnHide(IHyApplication &hyApp) override;
	virtual void OnResizeRenderer() override;
};

#endif // PROJECTDRAW_H
