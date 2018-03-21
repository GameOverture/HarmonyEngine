#pragma once

#include "Harmony/HyEngine.h"

class CheckerGrid : public HyPrimitive2d
{
	const glm::vec2		m_vDIMENSIONS;
	const float			m_fGRID_SIZE;

public:
	CheckerGrid(float fWidth, float fHeight, float fGridSize, HyEntity2d *pParent);
	virtual ~CheckerGrid();

	virtual void OnUpdateUniforms() override;
	virtual void OnWriteVertexData(char *&pRefDataWritePos) override;
};

