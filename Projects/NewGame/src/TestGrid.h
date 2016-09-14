#pragma once

#include "Harmony/HyEngine.h"

class TestGrid : public HyPrimitive2d
{
public:
	TestGrid();
	virtual ~TestGrid();

	virtual void OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef);
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

