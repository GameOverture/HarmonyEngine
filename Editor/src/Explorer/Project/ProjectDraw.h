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

#include "Global.h"

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

class ProjectDraw : public HyEntity2d
{
	CheckerGrid			m_CheckerGrid;
	HyShader *			m_pCheckerGridShader;

	HyPrimitive2d		m_Origin;

	OverGrid			m_OverGrid;
	HyShader *			m_pOverGridShader;

	// Misc shaders (used elsewhere than ProjectDraw)
	HyShader *			m_pTileMapSquareShader;
	HyShader *			m_pTileMapHalfSquareShader;
	HyShader *			m_pTileMapIsometricShader;
	HyShader *			m_pTileMapHexShader;

public:
	ProjectDraw();
	virtual ~ProjectDraw();

	void EnableGridBackground(bool bEnable);
	void EnableGridOrigin(bool bEnable);
	void EnableGridOverlay(bool bEnable);
	
	void OnResizeRenderer();
	void OnCameraUpdated();

	HyShader *GetTileMapGridShader(HyTileMapLayout eLayout) const;
};

#endif // PROJECTDRAW_H
