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

	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;
};

class OverGrid : public CheckerGrid
{
public:
	OverGrid(float fWidth, float fHeight, float fGridSize);
	virtual ~OverGrid();

	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;
};

class ProjectDraw : public IDraw
{
	CheckerGrid			m_CheckerGrid;
	HyShader *			m_pCheckerGridShader;

	HyPrimitive2d		m_OriginHorz;
	HyPrimitive2d		m_OriginVert;

	OverGrid			m_OverGrid;
	HyShader *			m_pOverGridShader;

public:
	ProjectDraw();
	virtual ~ProjectDraw();

	void EnableOrigin(bool bEnable);
	void EnableOverGrid(bool bEnable);
	
protected:
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
	virtual void OnZoom(HyZoomLevel eZoomLevel) override;
};

#endif // PROJECTDRAW_H
