#pragma once

#include "Harmony/HyEngine.h"

class %HY_CLASS% : public HyEngine
{
	HyCamera2d *	m_pCamera;

public:
	%HY_CLASS%(HarmonyInit &initStruct);
	virtual ~%HY_CLASS%();

	virtual bool OnUpdate() override;
};
