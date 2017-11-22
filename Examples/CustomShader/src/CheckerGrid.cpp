#include "CheckerGrid.h"


CheckerGrid::CheckerGrid(float fWidth, float fHeight, float fGridSize, HyEntity2d *pParent) :	HyPrimitive2d(pParent),
																								m_vDIMENSIONS(fWidth, fHeight),
																								m_fGRID_SIZE(fGridSize)
{
	GetShape().SetAsBox(m_vDIMENSIONS.x * 0.5f, m_vDIMENSIONS.y * 0.5f);
}

CheckerGrid::~CheckerGrid()
{
}

/*virtual*/ void CheckerGrid::OnUpdateUniforms()
{
	glm::mat4 mtx;
	HyPrimitive2d::GetWorldTransform(mtx);

	m_ShaderUniforms.Set("u_mtxTransform", mtx);
	m_ShaderUniforms.Set("u_fGridSize", m_fGRID_SIZE);
	m_ShaderUniforms.Set("u_vDimensions", m_vDIMENSIONS);
	m_ShaderUniforms.Set("u_vGridColor1", glm::vec4(106.0f / 255.0f, 105.0f / 255.0f, 113.0f / 255.0f, 1.0f));
	m_ShaderUniforms.Set("u_vGridColor2", glm::vec4(93.0f / 255.0f, 93.0f / 255.0f, 97.0f / 255.0f, 1.0f));
}

/*virtual*/ void CheckerGrid::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	HyAssert(GetNumVerts() == 6, "CheckerGrid::OnWriteDrawBufferData is trying to draw a primitive that's not a quad");
	
	for(int i = 0; i < 6; ++i)
	{
		*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = m_pVertBuffer[i];
		pRefDataWritePos += sizeof(glm::vec2);

		glm::vec2 vUV;
		switch(i)
		{
		case 0:
		case 3:
			vUV.x = 0.0f;
			vUV.y = 1.0f;
			break;

		case 1:
			vUV.x = 1.0f;
			vUV.y = 1.0f;
			break;

		case 2:
		case 4:
			vUV.x = 1.0f;
			vUV.y = 0.0f;
			break;

		case 5:
			vUV.x = 0.0f;
			vUV.y = 0.0f;
			break;
		}

		*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
		pRefDataWritePos += sizeof(glm::vec2);
	}
}
