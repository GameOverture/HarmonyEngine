#pragma once

#include "Harmony/HyEngine.h"
#include "CheckerGrid.h"

class CustomShader : public IHyApplication
{
	HyCamera2d *	m_pCamera;

	CheckerGrid		m_CheckerGrid;
	HyShader *		m_pCheckerGridShader;

public:
	CustomShader(HarmonyInit &initStruct);
	virtual ~CustomShader();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
