#pragma once

#include "Harmony/HyEngine.h"

class TestGrid : public HyPrimitive2d
{
	glm::vec2		m_Resolution;
public:
	TestGrid();
	virtual ~TestGrid();

	void SetResolution(int iWidth, int iHeight);

	virtual void OnUpdateUniforms();
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

