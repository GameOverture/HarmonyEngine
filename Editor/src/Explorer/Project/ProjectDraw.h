/**************************************************************************
 *	ProjectDraw.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECTDRAW_H
#define PROJECTDRAW_H

#include "IDraw.h"

class CheckerGrid : public HyPrimitive2d
{
protected:
	const glm::vec2		m_vDIMENSIONS;
	float				m_fGridSize;

public:
	CheckerGrid(float fWidth, float fHeight, float fGridSize);
	virtual ~CheckerGrid();

	virtual void OnUpdateUniforms() override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef) override;
};

class OverGrid : public CheckerGrid
{
public:
	OverGrid(float fWidth, float fHeight, float fGridSize);
	virtual ~OverGrid();

	virtual void OnUpdateUniforms() override;
};

class ProjectDraw : public IDraw
{
	CheckerGrid			m_CheckerGrid;
	HyShader *			m_pCheckerGridShader;

	OverGrid			m_OverGrid;
	HyShader *			m_pOverGridShader;

public:
	ProjectDraw();
	virtual ~ProjectDraw();

	void EnableOverGrid(bool bEnable);
	
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
};

#endif // PROJECTDRAW_H
